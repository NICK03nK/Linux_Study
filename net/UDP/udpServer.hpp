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
#include <functional>

using namespace std;

namespace Server
{
    static const string defaultIP = "0.0.0.0";
    static const int num = 1024;

    enum {USE_ERROR = 1, SOCK_ERROR, BIND_ERROR, OPEN_ERROR};

    class udpServer
    {
        using func_t = function<void(int, string, uint16_t, string)>;

    public:
        udpServer(const func_t& callBack, const uint16_t port, const string& ip = defaultIP)  // 构造对象时ip不需要传递，使用缺省值即可
            :_callBack(callBack)
            , _port(port)
            , _ip(ip)
            , _sockFd(-1)
        {}

        void initServer()
        {
            // 1. 调用socket()创建socket
            _sockFd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockFd == -1)
            {
                cerr << "socket error" << errno << " : " << strerror(errno) << endl;
                exit(SOCK_ERROR);
            }

            // 2. 调用bind()来将socket与ip和port进行绑定
            // 2.1. 创建一个sockaddr_in对象，并填写结构体成员
            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);  // 服务器与客户端进行通信时要将自己的ip和port一同通过网络传输给客户端，所以要将port网络化处理
            // 这里填ip使用缺省值，任意地址bind
            local.sin_addr.s_addr = inet_addr(_ip.c_str());  // 调用inet_addr()将点分十进制的ip处理成4字节ip并网络化

            // 2.2. 调用bind()
            int ret = bind(_sockFd, (struct sockaddr*)&local, sizeof(local));
            if (ret == -1)  // bind()失败返回值为-1
            {
                cerr << "bind error" << errno << " : " << strerror(errno) << endl;
                exit(BIND_ERROR);
            }
        }

        void start()
        {
            // 服务器程序启动后实际上是一个死循环，一直循环检测客户端的请求
            char buffer[num] = { 0 };
            for (;;)
            {
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                ssize_t ret = recvfrom(_sockFd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&peer, &len);
                
                if (ret > 0)
                {
                    buffer[ret] = 0;
                    string client_ip = inet_ntoa(peer.sin_addr);
                    uint16_t client_port = ntohs(peer.sin_port);
                    string message = buffer;

                    cout << client_ip << "[" << client_port << "]# " << message << endl;

                    _callBack(_sockFd, client_ip, client_port, message);
                }
            }
        }

        ~udpServer()
        {}

    private:
        uint16_t _port;  // 该服务进程在服务器主机中对应的端口号
        string _ip;  // 服务器主机的ip
        int _sockFd;  // 用于接收socket()的返回值(即一个文件描述符)
        func_t _callBack;  // 服务器进程对应的业务逻辑
    };
}