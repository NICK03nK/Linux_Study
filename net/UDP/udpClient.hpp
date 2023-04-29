#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <strings.h>
#include <netinet/in.h>

using namespace std;

namespace Client
{
    enum {USE_ERROR = 1, SOCK_ERROR, BIND_ERROR};

    class udpClient
    {
    public:
        udpClient(const string& ip, const uint16_t port)
            :_ip(ip)
            , _port(port)
            , _sockFd(-1)
            , _quit(false)
        {}

        void initClient()
        {
            // 调用socket()创建socket
            _sockFd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockFd == -1)
            {
                cerr << "socket error" << errno << " : " << strerror(errno) << endl;
                exit(SOCK_ERROR);
            }

            // client不需要显示bind()，OS会自动调用bind()
        }

        void run()
        {
            struct sockaddr_in server;
            bzero(&server, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(_port);
            server.sin_addr.s_addr = inet_addr(_ip.c_str());

            string message;
            while (!_quit)
            {
                cout << "Enter# ";
                cin >> message;

                sendto(_sockFd, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));
            }
        }

        ~udpClient()
        {}

    private:
        string _ip;
        uint16_t _port;
        int _sockFd;

        bool _quit;
    };
}