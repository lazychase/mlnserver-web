#pragma once

#include <cstdint>
#include <string>
#include <struct_mapping/struct_mapping.h>

#include "confMysql.h"
#include "confRedis.h"

namespace mapStruct {

    struct DBConnector
    {
        std::string userId;
        std::string userPw;
        std::string host;
        std::string dbName;

        static void regist() {
            struct_mapping::reg(&DBConnector::userId, "userId");
            struct_mapping::reg(&DBConnector::userPw, "userPw");
            struct_mapping::reg(&DBConnector::host, "host");
            struct_mapping::reg(&DBConnector::dbName, "dbName");
        }
    };

    struct Config
    {
        uint32_t version;
        uint32_t MYAPP_HTTP_PORT = 0;

        REDIS_CONF redisConf;
        MYSQL_CONF mysqlConf;


        static void initOrm() {
            struct_mapping::reg(&Config::version, "version");
            struct_mapping::reg(&Config::MYAPP_HTTP_PORT, "MYAPP_HTTP_PORT", struct_mapping::Default{ 0 });
            struct_mapping::reg(&Config::redisConf, "REDIS_CONF");
            struct_mapping::reg(&Config::mysqlConf, "MYSQL_CONF");

            // init redis Conf.
            []() {
                struct_mapping::reg(&REDIS_CONF::REDIS_HOST, "REDIS_HOST");
                struct_mapping::reg(&REDIS_CONF::REDIS_PORT, "REDIS_PORT", struct_mapping::Default{ 6379 });
                struct_mapping::reg(&REDIS_CONF::REDIS_PASSWD, "REDIS_PASSWD");
                struct_mapping::reg(&REDIS_CONF::REDIS_USE_REPLICA, "REDIS_USE_REPLICA", struct_mapping::Default{ false });
                struct_mapping::reg(&REDIS_CONF::REDIS_USE_CLUSTER, "REDIS_USE_CLUSTER", struct_mapping::Default{ false });
                struct_mapping::reg(&REDIS_CONF::REDIS_SOCKET_TIMEOUT_MS, "REDIS_SOCKET_TIMEOUT_MS", struct_mapping::Default{ 300 });
                struct_mapping::reg(&REDIS_CONF::REDIS_SUB_CHANNELS, "REDIS_SUB_CHANNELS");
            }();

            // init mysql Conf.
            []() {
                struct_mapping::reg(&MYSQL_CONF::MYSQL_HOST, "MYSQL_HOST");
                struct_mapping::reg(&MYSQL_CONF::MYSQL_PORT, "MYSQL_PORT", struct_mapping::Default{ 3306 });
                struct_mapping::reg(&MYSQL_CONF::MYSQL_USER, "MYSQL_USER");
                struct_mapping::reg(&MYSQL_CONF::MYSQL_PASSWD, "MYSQL_PASSWD");
                struct_mapping::reg(&MYSQL_CONF::MYSQL_DBNAME, "MYSQL_DBNAME");
                struct_mapping::reg(&MYSQL_CONF::MYSQL_CONN_POOL_MAX, "MYSQL_CONN_POOL_MAX");
            }();
        }
    };

}//namespace mappingStruct {



class Configuration
{
public:
    static Configuration& instance() { static Configuration _instance; return _instance; }

    mapStruct::Config& load(const char* path = "netconfig.json");

public:
    mapStruct::Config _config;
};
