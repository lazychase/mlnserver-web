#include "mysqlConnMgr.h"

#include "mysqlConn.h"
#include <cassert>

#include "asioContext.h"

#include <boost/mysql.hpp>
#include <boost/asio/ssl/context.hpp>
#include <net/logger.hpp>



#include <boost/mysql/field_view.hpp>
#include <boost/mysql/handshake_params.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>


using namespace MBMySQL;

ConnMgr::~ConnMgr()
{
	for (SessionPool& pool : _pools) {
		
		while (!pool.empty()) {
			ConnPtr conn = pool.top();
			pool.pop();

			conn->close();
		}
	}
	_pools.clear();
}

void ConnMgr::createPoolFunc(const int poolCnt, const size_t targetIdx, SessionPool& pool)
{
	for (uint32_t i = 0; i < poolCnt; ++i) {
		pool.push(createConn(targetIdx));
	}
}

size_t ConnMgr::initTarget(const MYSQL_CONF& conf)
{
	static size_t targetIdx = 0;

	_opts.push_back(conf);
	_pools.push_back(std::stack< ConnPtr >());
	SessionPool& pool = _pools.at(_pools.size() - 1);

	/*for (uint32_t i = 0; i < conf.MYSQL_CONN_POOL_MAX; ++i) {
		pool.push(createConn(targetIdx));
	}*/
	std::thread{ std::bind(&ConnMgr::createPoolFunc, this, conf.MYSQL_CONN_POOL_MAX, targetIdx, pool) }.detach();

	return targetIdx++;
}

MBMySQL::ConnPtr ConnMgr::getConn(const size_t targetIdx /*= 0*/)
{
	{
		std::lock_guard<std::mutex> lock(_mutex);

		assert(targetIdx < _pools.size());

		auto& sessionPool = _pools.at(targetIdx);

		// 생성해둔 세션이 있다면 반환합니다.
		if (false == sessionPool.empty()) {
			ConnPtr conn = sessionPool.top();
			sessionPool.pop();

			return conn;
		}
	}

	// 연결해둔 세션이 없다면 연결을 맺고 반환합니다.
	return createConn(targetIdx);
}

ConnPtr ConnMgr::createConn(const size_t targetIdx)
{
	assert(targetIdx < _opts.size());

	const MYSQL_CONF& conf = _opts.at(targetIdx);

	try {
		//g_ioc
		boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tls_client);
		auto conn = std::make_shared<boost::mysql::tcp_ssl_connection>(*g_ioc.get(), ssl_ctx);

		// Connection params
		boost::mysql::handshake_params params(
			conf.MYSQL_USER,
			conf.MYSQL_PASSWD,
			conf.MYSQL_DBNAME // database to use; leave empty or omit for no database
		);

		// Resolver for hostname resolution
		boost::asio::ip::tcp::resolver resolver(g_ioc->get_executor());

		// Invoke the resolver's appropriate function to perform the resolution.
		auto endpoints = resolver.resolve(conf.MYSQL_HOST, std::to_string(conf.MYSQL_PORT));


		/**
		 * Before using the connection, we have to connect to the server by:
		 *    - Establishing the TCP-level session.
		 *    - Authenticating to the MySQL server. The SSL handshake is performed as part of this.
		 * connection::connect takes care of both.
		 */
		conn->connect(*endpoints.begin(), params);
		return conn;
	}
	catch (const boost::system::system_error& err)
	{
		LOGE("Failed createConn() boostError. errMsg:{}, errCode:{}", err.what(), err.code().value());
		return ConnPtr(nullptr);
	}
	catch (const std::exception& err)
	{
		LOGE("Failed createConn(). errMsg:{}", err.what());
		return ConnPtr(nullptr);
	}
}


bool ConnMgr::releaseConn(ConnPtr)
{
	return true;
}