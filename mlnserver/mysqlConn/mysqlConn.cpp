#include "mysqlConn.h"

using namespace MBMySQL;

Conn::Conn(const size_t targetIdx, const MYSQL_CONF& poolOpts)
	: _conf(poolOpts)
{
	_targetIdx = targetIdx;
}

Conn::~Conn()
{
}

void Conn::connect()
{
}