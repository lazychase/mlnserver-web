#pragma once


#include <cstdint>
#include <string>
#include <vector>

// REDIS_CONF ��ü�� ORM ����ؼ� ������Դϴ�.
// ��� �۾��� Configuration ������ mapStruct::Config ��ü���� �������Դϴ�.
// ����� �߰��� ��� orm �������ּ���.

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
