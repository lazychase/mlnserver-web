#include "sessionManager.h"
#include <fmt/format.h>

#include "redisConnector.h"
#include <net/logger.hpp>


void SessionManager::init(const uint32_t sessionExpireSec /*= 30 * 60*/)
{
	_sessionExpireTimeSec = sessionExpireSec;
}


inline 
std::string SessionManager::genSessionKey(const uint64_t mbuid)
{
	return fmt::format("mbsk{}", mbuid);
}


SessionManager::SessionData  SessionManager::createSession(const uint64_t mbuid)
{
	return SessionData{
		mbuid
		, std::move(genSessionKey(mbuid))
		, _sessionExpireTimeSec
	};
}

std::tuple<bool, std::optional<SessionManager::SessionData> > 
SessionManager::isValidSession(const std::string& sessionKey)
{
	RedisConnector::FieldValueCont fieldValues;

	if (false == RedisConnector::instance().hgetAll(sessionKey, fieldValues) ){
		return { false, std::nullopt };
	}
	else {
		SessionManager::SessionData sessionData;
		try {
			sessionData.mbuid = std::stoull(fieldValues["mbuid"]);
			sessionData.sessionKey = sessionKey;
		}
		catch (std::exception& e) {
			LOGC("failed parsing-sessionData. sessionKey:{}. msg:{}", sessionKey, e.what());
			return { false, std::nullopt };
		}
		return { true, sessionData };
	}
}

