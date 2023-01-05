#pragma once


#include <cstdint>
#include <string>
#include <vector>

// 객체는 ORM 등록해서 사용중입니다.
// 등록 작업은 Configuration 파일의 mapStruct::Config 객체에서 수행중입니다.
// 멤버를 추가할 경우 orm 연결해주세요.

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