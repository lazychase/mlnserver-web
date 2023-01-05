#pragma once


#include <cstdint>
#include <string>
#include <vector>

// ��ü�� ORM ����ؼ� ������Դϴ�.
// ��� �۾��� Configuration ������ mapStruct::Config ��ü���� �������Դϴ�.
// ����� �߰��� ��� orm �������ּ���.

struct MYSQL_CONF
{
	std::string MYSQL_HOST;
	uint32_t MYSQL_PORT;
	std::string MYSQL_USER;
	std::string MYSQL_PASSWD;
	std::string MYSQL_DBNAME;
	uint32_t MYSQL_CONN_POOL_MAX;

	/*const unsigned int pool_limit = 1,
		const unsigned int expire_idle_timeout_sec = 0*/
};