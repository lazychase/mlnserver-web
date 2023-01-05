#include "lobbyServerCoro.h"
#include "asioContext.h"
#include <boost/json.hpp>

#include "lobbyServerCoroInclude.h"
#include "configuration.h"

#include "stringUtils.hpp"
#include "lobbyServerUtils.h"

#include "myPacketHandler/handleAccount.h"
#include "myPacketHandler/handlePlayer.h"


using namespace mln::net;

using MBMySQL::ConnMgr;
using MBMySQL::ConnPtr;

using boost::json::value;
using boost::json::serialize;

using boost::mysql::error_code;
using boost::mysql::error_info;
using boost::mysql::tcp_ssl_resultset;

const static size_t DB_TARGET_LOBBY = 0;


#include "lobbyServerMacros.h"

void LobbyServerCoro::start(const unsigned int port)
{
	auto& conf = Configuration::instance().load();
	ConnMgr::instance().initTarget(conf.mysqlConf);

	auto& httpSvr = mln::net::HttpServerCoro::instance();

	registLobbyHandlerAccount(httpSvr);
	registLobbyHandlerPlayer(httpSvr);

	httpSvr.startAsync(port, g_ioc);
}


void LobbyServerCoro::run(const unsigned int port, const size_t threads /*= 3*/)
{
	auto& conf = Configuration::instance().load();
	ConnMgr::instance().initTarget(conf.mysqlConf);

	auto& httpSvr = mln::net::HttpServerCoro::instance();

	registLobbyHandlerAccount(httpSvr);
	registLobbyHandlerPlayer(httpSvr);

	httpSvr.acceptAsync(port, g_ioc, threads);
}


