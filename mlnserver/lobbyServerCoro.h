#pragma once

class LobbyServerCoro
{
public:
    static LobbyServerCoro& instance() { static LobbyServerCoro _instance; return _instance; }

    void start(const unsigned int port);
    void run(const unsigned int port, const size_t threads = 3);
};
