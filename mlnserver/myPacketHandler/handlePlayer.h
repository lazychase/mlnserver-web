#pragma once

#include "../lobbyServerCoroInclude.h"

using mln::net::HeaderMap;
using mln::net::send_lambda;

extern void registLobbyHandlerPlayer(mln::net::HttpServerCoro& httpSvr);

HTTP_CORO_HANDLER_PROTO(controlValueSet);
HTTP_CORO_HANDLER_PROTO(controlValueGet);
