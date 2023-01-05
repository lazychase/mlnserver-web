#pragma once

#include "confRedis.h"
#include <optional>
#include <unordered_map>


class RedisConnector
{
public:
    using FieldValueCont = std::unordered_map<std::string, std::string>;

    static RedisConnector& instance() { static RedisConnector _instance; return _instance; }

    void init(const REDIS_CONF& redisConf);
    void publish(const std::string& channelName, const std::string& msg);


    void hset(const std::string & key, const std::string & field, const std::string & value, const uint32_t expireSec = 0);
    void hset(const std::string & key, const FieldValueCont & fieldValues, const uint32_t expireSec = 0);

    bool hgetAll(const std::string& key, FieldValueCont& outFieldValues);

    

    static void testRedis();

};//class RedisConnector
