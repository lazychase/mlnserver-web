#pragma once

#include <cstdint>
#include <string>
#include <optional>

class SessionManager
{
public:
	static SessionManager& instance() { static SessionManager _instance; return _instance; }

	struct SessionData
	{
		uint64_t mbuid;
		std::string sessionKey;
		uint32_t expireSec = 0;
	};

	void init(const uint32_t sessionExpireSec);

	SessionData createSession(const uint64_t mbuid);
	static std::string genSessionKey(const uint64_t mbuid);

	std::tuple<bool, std::optional<SessionData> > isValidSession(const std::string& sessionKey);

private:
	uint32_t _sessionExpireTimeSec = 30 * 60;
};