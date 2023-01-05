#pragma once


#include <cstdint>
#include <string>
#include <vector>

// REDIS_CONF 객체는 ORM 등록해서 사용중입니다.
// 등록 작업은 Configuration 파일의 mapStruct::Config 객체에서 수행중입니다.
// 멤버를 추가할 경우 orm 연결해주세요.

struct REDIS_CONF
{
	std::string REDIS_HOST;
	uint32_t REDIS_PORT;
	std::string REDIS_PASSWD;
	bool REDIS_USE_REPLICA;
	bool REDIS_USE_CLUSTER;
	uint32_t REDIS_SOCKET_TIMEOUT_MS;
	uint32_t REDIS_MAX_CONNECTIONS;
	std::vector<std::string> REDIS_SUB_CHANNELS;
};
