#pragma once

#include "../lobbyServerCoroInclude.h"

using mln::net::HeaderMap;
using mln::net::send_lambda;

extern void registLobbyHandlerAccount(mln::net::HttpServerCoro & httpSvr);

HTTP_CORO_HANDLER_PROTO(controlAlive);
HTTP_CORO_HANDLER_PROTO(controlAliveSleep);
HTTP_CORO_HANDLER_PROTO(controlAccountPass);