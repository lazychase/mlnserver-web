#include "handleAccount.h"

#include "lobbyServerUtils.h"
#include "../stringUtils.hpp"
#include "gameDefinitions.h"
#include "redisConnector.h"
#include "tableManager.h"
#include <uuidxx/uuidxx.hpp>


using MBMySQL::ConnMgr;
using MBMySQL::ConnPtr;
using boost::json::value;
using boost::json::serialize;
using boost::mysql::error_code;
using boost::mysql::error_info;
using boost::mysql::tcp_ssl_resultset;

using namespace mlnserver::stringUtils;
using namespace mlnserver::httpCoro;


void registLobbyHandlerAccount(mln::net::HttpServerCoro & httpSvr)
{
	httpSvr.registHandler("/alive", controlAlive);
	httpSvr.registHandler("/account/auth_pass", controlAccountPass);
}

HTTP_CORO_HANDLER_PROTO(controlAlive)
{
	SEND_RSP(R"({"msg":"hello~"})");
}

HTTP_CORO_HANDLER_PROTO(controlAliveSleep)
{
	std::this_thread::sleep_for(std::chrono::seconds(5));
	SEND_RSP(R"({"msg":"Sleep~"})");
}

HTTP_CORO_HANDLE_CASUAL_BEGIN(controlAccountPass)
{
	CASUAL_OBJ_MOVE_AT_STRING(user_id);

	ConnPtr connObj = ConnMgr::instance().getConn();

	auto sql = fmt::format("SELECT mbuid, proceeded FROM account WHERE user_id='{}'"
		, user_id
	);

	tcp_ssl_resultset result;
	sendQuery(connObj, sender, sql, result);

	uint64_t mbuid;
	boost::mysql::row row;
	if (false == readOne(sender, result, row)) {
		auto sqlInsert = fmt::format("INSERT INTO account(user_id, user_id_ext, proceeded) VALUES('{}', '{}', 0);"
			, user_id
			, std::move(uuidxx::uuid::Generate().ToString())
		);
		tcp_ssl_resultset resultInsert;
		purgeResult(sender, result);
		sendQuery(connObj, sender, sqlInsert, resultInsert);

		mbuid = resultInsert.last_insert_id();
	}
	else {
		purgeResult(sender, result);

		mbuid = row[0].as_uint64();
	}


	// 임시 인증 권한을 발급합니다.
	SessionManager::SessionData sessionData = std::move(SessionManager::instance().createSession(mbuid));

	std::unordered_map<std::string, std::string> fieldValues{
		{"mbuid", std::move(std::to_string(sessionData.mbuid))}
	};
	// 발급한 세션을 레디스에 저장합니다.
	RedisConnector::instance().hset(sessionData.sessionKey, fieldValues, sessionData.expireSec);

	value resJv{
		{"msg","ok"},
		{"mbuid", mbuid},
		{"proceeded", 7},
		{"symmetricKey", ""},
		{"sessionKey", sessionData.sessionKey},
		{"sessionExpireSec", 60 * 30}
	};

	SEND_RSP(serialize(resJv));
}
HTTP_CORO_HANDLE_CASUAL_END


