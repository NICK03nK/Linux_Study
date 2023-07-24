#pragma once

#include <iostream>
#include "sock.hpp"

namespace Server
{
    static const uint16_t defaultPort = 8080;

    class selectServer
    {
    public:
        selectServer(uint16_t port = defaultPort)
            :_port(port)
            , _listenSockFd(-1)
        {}

        void initServer()
        {
            _listenSockFd = Sock::Socket();
            Sock::Bind(_listenSockFd, _port);
            Sock::Listen(_listenSockFd);
        }

        void start()
        {
            while (true)
            {
                std::string client_ip;
                uint16_t client_port;
                int sockFd = Sock::Accept(_listenSockFd, &client_ip, &client_port);
                if (sockFd == -1)
                {
                    continue;
                }

                // 开始进行服务器的处理逻辑
                
            }
        }

        ~selectServer()
        {}

    private:
        uint16_t _port;  // 服务器端口号
        int _listenSockFd;  // 监听套接字
    };
}