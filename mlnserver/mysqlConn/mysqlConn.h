#pragma once

#include "../confMysql.h"

namespace MBMySQL {

	class Conn
	{
	public:
		enum class status : unsigned char {
			closed = 1,
			connecting = 2,
			connected = 3,
			using_now = 4
		};

	public:
		Conn() = default;
		Conn(const size_t targetIdx, const MYSQL_CONF& poolOpts);
		~Conn();

		void connect();


	public:
		size_t _targetIdx;
		const MYSQL_CONF& _conf;

		status _status = status::closed;
	
	};//class Conn

};//namespace MBMySQL {
