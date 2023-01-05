#ifndef MB_LOBBY_SERVER_CORO_MACROS
#define MB_LOBBY_SERVER_CORO_MACROS


#define HTTP_CORO_MYSQL_TEMPLATE_CASUAL_BEGIN	\
mysqlTemplate(\
	std::move(header)\
	, std::move(bodyString)\
	, sender\
	, keep_alive\
	, version\
	, sessionDataOpt\
	, [](\
	HeaderMap&& header\
	, boost::json::value& jv\
	, send_lambda& sender\
	, bool keep_alive\
	, unsigned int version\
	, std::optional<SessionManager::SessionData> sessionDataOpt = std::nullopt\
)

#define HTTP_CORO_MYSQL_TEMPLATE_SESSION_BEGIN	\
mysqlTemplate(\
	std::move(header)\
	, std::move(bodyString)\
	, sender\
	, keep_alive\
	, version\
	, sessionDataOpt\
	, [](\
	HeaderMap&& header\
	, boost::json::value& jv\
	, send_lambda& sender\
	, bool keep_alive\
	, unsigned int version\
	, std::optional<SessionManager::SessionData> sessionDataOpt = std::nullopt\
)

#define HTTP_CORO_MYSQL_TEMPLATE_END	);








#ifndef OBJ_STRING_AT
#define OBJ_STRING_AT(__key)	value_to<std::string>(jv.at(__key))
#endif//#ifndef OBJ_STRING_AT

#ifdef METABOLOTS_USE_PARAM_CHECKER
#define SAVE_METABOLOTS_PARAM(p)	paramStringForDev=#p;
#else
#define SAVE_METABOLOTS_PARAM(p)	((void)(0));
#endif


#ifndef SEND_RSP
#define SEND_RSP(msg)	\
sender(std::move(getRsponse(msg, keep_alive, version)))

#define SEND_RSP_RESULT(msg, resultString)	\
sender(std::move(getRsponse(msg, keep_alive, version, resultString)))
#endif



#ifndef OBJ_MOVE_AT_STRING
#define OBJ_MOVE_AT_STRING(__key)	std::move(OBJ_STRING_AT(__key)); SAVE_METABOLOTS_PARAM(__key)
#endif

#ifndef CASUAL_OBJ_MOVE_AT_STRING
#define CASUAL_OBJ_MOVE_AT_STRING(__key)	SAVE_METABOLOTS_PARAM(__key); std::string __key = std::move(OBJ_STRING_AT(#__key))
#endif










#define HTTP_CORO_HANDLER_PROTO(funcName)	\
void funcName(\
  HeaderMap&& header\
, std::string&& bodyString\
, send_lambda&& sender\
, bool keep_alive\
, unsigned int version\
)

#define HTTP_CORO_HANDLE_CASUAL_BEGIN(__handler_name__)	\
HTTP_CORO_HANDLER_PROTO(__handler_name__)\
{\
	std::optional<SessionManager::SessionData> sessionDataOpt;\
	HTTP_CORO_MYSQL_TEMPLATE_CASUAL_BEGIN

#define HTTP_CORO_HANDLE_CASUAL_END	\
	HTTP_CORO_MYSQL_TEMPLATE_END\
}




#define HTTP_CORO_SESSION_CHECK	\
const auto itSessionKeyIt = header.find("mbsk");\
if (header.end() == itSessionKeyIt) {\
	SEND_RSP_RESULT(R"({"msg":"not found session"})", "1");\
	return;\
}\
auto [isSessionValid, sessionDataOpt] = SessionManager::instance().isValidSession(itSessionKeyIt->second);\
if (false == isSessionValid) {\
	SEND_RSP_RESULT(R"({"msg":"invalid session"})", "1");\
	return;\
}

#define HTTP_CORO_HANDLE_SESSION_BEGIN(__handler_name__)	\
HTTP_CORO_HANDLER_PROTO(__handler_name__)\
{\
	HTTP_CORO_SESSION_CHECK\
	HTTP_CORO_MYSQL_TEMPLATE_SESSION_BEGIN

#define HTTP_CORO_HANDLE_SESSION_END	\
	HTTP_CORO_MYSQL_TEMPLATE_END\
}



#endif//#define MB_LOBBY_SERVER_CORO_MACROS