#pragma once

#include <iostream>
#include <sys/select.h>
#include <cstring>
#include <string>
#include <functional>
#include "sock.hpp"

namespace Server
{
    static const uint16_t defaultPort = 8080;
    static const int fdNum = sizeof(fd_set) * 8;
    static const int defaultFd = -1;

    using func_t = std::function<std::string(const std::string&)>;

    class selectServer
    {
    public:
        selectServer(func_t func, uint16_t port = defaultPort)
            :_port(port)
            , _listenSockFd(-1)
            , _fdarray(nullptr)
            , _func(func)
        {}

        void initServer()
        {
            _listenSockFd = Sock::Socket();
            Sock::Bind(_listenSockFd, _port);
            Sock::Listen(_listenSockFd);

            _fdarray = new int[fdNum];
            for (int i = 0; i < fdNum; ++i)
            {
                _fdarray[i] = defaultFd;
            }

            _fdarray[0] = _listenSockFd;
        }

        void Print()
        {
            std::cout << "fd list:";
            for (int i = 0; i < fdNum; ++i)
            {
                if (_fdarray[i] != defaultFd)
                {
                    std::cout << " " << _fdarray[i];
                }
            }
            std::cout << std::endl;
        }

        void Accepter(int listenSockFd)
        {
            // select()告诉我们，_listenSockFd的读事件已就绪
            std::string client_ip;
            uint16_t client_port;
            int sockFd = Sock::Accept(listenSockFd, &client_ip, &client_port);

            if (sockFd < 0)
            {
                return;
            }
            else
            {
                logMessage(NORMAL, "accept success, ip:port->[%s:%d]", client_ip.c_str(), client_port);
            }

            // 获取的新的sockFd不能直接进行网络操作，必须先托管给_fdarray
            int i = 0;
            for (; i < fdNum; ++i)
            {
                if (_fdarray[i] == defaultFd)
                {
                    break;
                }
            }
            if (i == fdNum) // 说明所能承载的fd已经满了
            {
                logMessage(WARNING, "server is full, please wait...");
                close(sockFd);
            }
            else // i < fdNum
            {
                _fdarray[i] = sockFd; // 将新获取的sockFd交给_fdarray托管
            }

            Print();
        }

        void Receiver(int sockFd, int pos)
        {
            // 1.读取request
            char buffer[1024] = { 0 };
            ssize_t n = recv(sockFd, buffer, sizeof(buffer) - 1, 0);
            
            if (n > 0)
            {
                buffer[n] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (n == 0)  // recv()返回值为0，即客户端关闭了文件描述符
            {
                logMessage(NORMAL, "client quit");

                _fdarray[pos] = defaultFd;
                close(sockFd);

                return;
            }
            else  // recv()出错
            {
                logMessage(ERROR, "client quit: %s", strerror(errno));

                _fdarray[pos] = defaultFd;
                close(sockFd);
                
                return;
            }

            // 2.处理request
            std::string response = _func(buffer);

            // 3.返回response
            write(sockFd, response.c_str(), response.size());
        }

        void HandlerEvent(const fd_set& rfds)
        {
            for (int i = 0; i < fdNum; ++i)
            {
                if (_fdarray[i] == defaultFd)  // 过滤掉非法的fd
                {
                    continue;
                }

                if (_fdarray[i] == _listenSockFd && FD_ISSET(_listenSockFd, &rfds))  // 监听套接字的读事件就绪，即有新的连接到来
                {
                    Accepter(_listenSockFd);  // 获取新连接
                }
                else if (FD_ISSET(_fdarray[i], &rfds))  // 普通的IO事件
                {
                    Receiver(_fdarray[i], i);
                }
            }
        }

        void start()
        {
            while (true)
            {
                fd_set rfds;
                FD_ZERO(&rfds);
                
                int maxFd = _fdarray[0];
                for (int i = 0; i < fdNum; ++i)
                {
                    if (_fdarray[i] != defaultFd)  // 合法的fd
                    {
                        FD_SET(_fdarray[i], &rfds); // 将合法的fd添加到读文件描述符集中
                    }

                    if (maxFd < _fdarray[i])  // 更新maxFd，找到所有fd中最大的fd，作为select()的第一个参数
                    {
                        maxFd = _fdarray[i];
                    }
                }
                logMessage(NORMAL, "maxFd is %d", maxFd);

                // 调用select()达到非阻塞的目的
                struct timeval timeout = {1, 0};  // 将timeout写到循环里，每次执行时都重新设置timeout
                // int ret = select(_listenSockFd + 1, &rfds, nullptr, nullptr, &timeout);
                int ret = select(maxFd + 1, &rfds, nullptr, nullptr, nullptr);
                switch (ret)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "select() error, code: %d, err string: %s", errno, strerror(errno));
                    break;
                default:
                    // 说明有事件就绪了
                    logMessage(NORMAL, "have an event ready");

                    HandlerEvent(rfds);

                    break;
                }

                // 开始进行服务器的处理逻辑
                
            }
        }

        ~selectServer()
        {
            if (_listenSockFd < 0)
            {
                close(_listenSockFd);
            }

            if (_fdarray)
            {
                delete[] _fdarray;  // _fdarray是new[]出来的，析构时要注意delete[]
            }
        }

    private:
        uint16_t _port;  // 服务器端口号
        int _listenSockFd;  // 监听套接字
        int* _fdarray;  // 管理fd的数组
        func_t _func;  // 该函数用于对收到的数据进行业务逻辑处理
    };
}