#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

namespace Client
{
    enum {USE_ERROR = 1, SOCK_ERROR};

    class tcpClient
    {
    public:
        tcpClient(const string& link_ip, const uint16_t& link_port)
            :_serverIp(link_ip)
            , _serverPort(link_port)
            , _sockFd(-1)
        {}

        void initClient()
        {
            // 创建套接字
            _sockFd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockFd == -1)
            {
                cerr << "create socket error" << endl;
                exit(SOCK_ERROR);
            }
        }

        void run()
        {
            // 与服务器建立链接
            struct sockaddr_in server;
            bzero(&server, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(_serverPort);
            server.sin_addr.s_addr = inet_addr(_serverIp.c_str());

            int ret = connect(_sockFd, (struct sockaddr*)&server, sizeof(server));
            if (ret == 0)  // 建立链接成功
            {
                string message;
                while (true)
                {
                    cout << "Enter# ";
                    getline(cin, message);

                    // 向服务器发消息
                    write(_sockFd, message.c_str(), message.size());

                    // 接收服务器发回的消息
                    char buffer[1024] = { 0 };
                    int n = read(_sockFd, buffer, sizeof(buffer) - 1);
                    if (n > 0)
                    {
                        buffer[n] = 0;
                        cout << "server$ " << buffer << endl;
                    }
                    else  // read()读取失败
                    {
                        break;
                    }
                }
            }
            else  // 建立链接失败
            {
                cerr << "socket connect error" << endl;
            }
        }

        ~tcpClient()
        {
            
        }

    private:
        string _serverIp;  // 要连接服务器的主机ip
        uint16_t _serverPort;  // 要连接服务的器程序的port
        int _sockFd;
    };
}