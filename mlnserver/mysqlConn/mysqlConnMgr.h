	#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <stack>
#include "../confMysql.h"

#include <boost/mysql.hpp>


namespace MBMySQL {

using ConnPtr = std::shared_ptr<boost::mysql::tcp_ssl_connection>;
using SessionPool = std::stack< ConnPtr >;

class ConnMgr 
{
public:
	static ConnMgr& instance() { static ConnMgr _instance; return _instance; }
	~ConnMgr();
			
	size_t initTarget(const MYSQL_CONF &conf);

	ConnPtr getConn(const size_t targetIdx = 0);
	bool releaseConn(ConnPtr);

private:
	ConnPtr createConn(const size_t targetIdx);
	void createPoolFunc(const int poolCnt, const size_t targetIdx, SessionPool& pool);

private:
	std::vector<SessionPool> _pools;
	std::vector<MYSQL_CONF> _opts;

	std::mutex _mutex;
};

};//namespace MBMySQL {