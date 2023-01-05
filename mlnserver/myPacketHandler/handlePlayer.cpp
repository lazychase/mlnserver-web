#include "handlePlayer.h"

#include "lobbyServerUtils.h"
#include "../stringUtils.hpp"
#include "redisConnector.h"

#include "tableManager.h"

using MBMySQL::ConnMgr;
using MBMySQL::ConnPtr;
using boost::json::value;
using boost::json::serialize;
using boost::mysql::error_code;
using boost::mysql::error_info;
using boost::mysql::tcp_ssl_resultset;

using namespace mlnserver::httpCoro;
using namespace mlnserver::stringUtils;


void registLobbyHandlerPlayer(mln::net::HttpServerCoro& httpSvr)
{
	httpSvr.registHandler("/player/value_set", controlValueSet);
	httpSvr.registHandler("/player/value_get", controlValueGet);
}


HTTP_CORO_HANDLE_SESSION_BEGIN(controlValueGet)
{
	CASUAL_OBJ_MOVE_AT_STRING(myValue);

	// MySQL 세션 획득
	ConnPtr connObj = ConnMgr::instance().getConn();

	auto sql = fmt::format("SELECT EXISTS(SELECT * FROM player WHERE player_name='{}') as RESULT;"
		, myValue
	);

	tcp_ssl_resultset result;
	sendQuery(connObj, sender, sql, result);

	boost::mysql::row row;
	readOne(sender, result, row);
	const int64_t exists = row[0].as_int64();

	purgeResult(sender, result);

	value resJv{
		{"msg","ok"},
		{"player_name",myValue},
		{"exists",exists},
		{"comment", std::move(makeComment(L"exists   0->해당 닉네임 없음. 1->해당 닉네임 존재.\
"))}
	};
	SEND_RSP(serialize(resJv));
}
HTTP_CORO_HANDLE_SESSION_END





HTTP_CORO_HANDLE_SESSION_BEGIN(controlValueSet)
{
	CASUAL_OBJ_MOVE_AT_STRING(myValue);

	ConnPtr connObj = ConnMgr::instance().getConn();

	std::string sql;
	tcp_ssl_resultset result;
	unsigned int attachedCnt = 0;

	sql = fmt::format("INSERT INTO player(mbuid, myvalue) VALUES({}, '{}')"
		, sessionDataOpt.value().mbuid
		, myValue
	);

	try {
		sendQuery(connObj, sender, sql, result);
	}
	catch (const boost::system::system_error& e) {
		std::string eWhat = e.what();

		if (std::string::npos != eWhat.find("dup_entry")) {
			SEND_INE(getServerError(keep_alive, version, R"({"msg":"exists myvalue"})"));
			return;
		}
		else {
			throw e;
		}
	}

	if (1 != result.affected_rows()) {
		// 결과는 무조건 닉네임 중복으로 보냅니다.
		value resJv{
			{"msg","existing nickname"},
			{"myvalue", myValue}
		};
		SEND_RSP_RESULT(serialize(resJv), "1");
		return;
	}

	value resJv{
		{"msg","ok"}
	};

	SEND_RSP(serialize(resJv));
}
HTTP_CORO_HANDLE_SESSION_END
