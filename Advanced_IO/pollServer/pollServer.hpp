#pragma once

#include <iostream>
#include <poll.h>
#include <cstring>
#include <string>
#include <functional>
#include "sock.hpp"

namespace Server
{
    static const uint16_t defaultPort = 8080;
    static const int num = 2048;
    static const int defaultFd = -1;

    using func_t = std::function<std::string(const std::string&)>;

    class pollServer
    {
    public:
        pollServer(func_t func, uint16_t port = defaultPort)
            :_port(port)
            , _listenSockFd(-1)
            , _poll_fds(nullptr)
            , _func(func)
        {}

        void initServer()
        {
            _listenSockFd = Sock::Socket();
            Sock::Bind(_listenSockFd, _port);
            Sock::Listen(_listenSockFd);

            _poll_fds = new struct pollfd[num];
            for (int i = 0; i < num; ++i)
            {
                ResetItem(i);
            }

            // 将listen套接字设置进fd数组中
            _poll_fds[0].fd = _listenSockFd;
            _poll_fds[0].events = POLLIN;
        }

        void Print()
        {
            std::cout << "fd list:";
            for (int i = 0; i < num; ++i)
            {
                if (_poll_fds[i].fd != defaultFd)
                {
                    std::cout << " " << _poll_fds[i].fd;
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

            // 获取的新的sockFd不能直接进行网络操作，必须先托管给_poll_fds
            int i = 0;
            for (; i < num; ++i)
            {
                if (_poll_fds[i].fd == defaultFd)
                {
                    break;
                }
            }
            if (i == num) // 说明所能承载的fd已经满了
            {
                logMessage(WARNING, "server is full, please wait...");
                close(sockFd);
            }
            else // i < fdNum
            {
                _poll_fds[i].fd = sockFd; // 将新获取的sockFd交给_poll_fds托管
                _poll_fds[i].events = POLLIN;
                _poll_fds[i].revents = 0;
            }

            Print();
        }

        void Receiver(int pos)
        {
            // 1.读取request
            char buffer[1024] = { 0 };
            ssize_t n = recv(_poll_fds[pos].fd, buffer, sizeof(buffer) - 1, 0);
            
            if (n > 0)
            {
                buffer[n] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (n == 0)  // recv()返回值为0，即客户端关闭了文件描述符
            {
                logMessage(NORMAL, "client quit");

                ResetItem(pos);

                close(_poll_fds[pos].fd);

                return;
            }
            else  // recv()出错
            {
                logMessage(ERROR, "client quit: %s", strerror(errno));

                ResetItem(pos);

                close(_poll_fds[pos].fd);
                
                return;
            }

            // 2.处理request
            std::string response = _func(buffer);

            // 3.返回response
            write(_poll_fds[pos].fd, response.c_str(), response.size());
        }

        void ResetItem(int pos)
        {
            _poll_fds[pos].fd = defaultFd;
            _poll_fds[pos].events = 0;
            _poll_fds[pos].revents = 0;
        }

        void HandlerEvent()
        {
            for (int i = 0; i < num; ++i)
            {
                if (_poll_fds[i].fd == defaultFd)  // 过滤掉非法的fd
                {
                    continue;
                }
                if (!(_poll_fds[i].events & POLLIN))
                {
                    continue;
                }

                if (_poll_fds[i].fd == _listenSockFd && _poll_fds[i].revents & POLLIN)  // 监听套接字的读事件就绪，即有新的连接到来
                {
                    Accepter(_listenSockFd);  // 获取新连接
                }
                else if (_poll_fds[i].revents & POLLIN)  // 普通的IO事件
                {
                    Receiver(i);
                }
                else
                {}
            }
        }

        void start()
        {
            int timeout = -1;
            while (true)
            {
                int ret = poll(_poll_fds, num, timeout);
                switch (ret)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "poll() error, code: %d, err string: %s", errno, strerror(errno));
                    break;
                default:
                    // 说明有事件就绪了
                    logMessage(NORMAL, "have an event ready");

                    HandlerEvent();

                    break;
                }
            }
        }

        ~pollServer()
        {
            if (_listenSockFd < 0)
            {
                close(_listenSockFd);
            }

            if (_poll_fds)
            {
                delete[] _poll_fds;  // _poll_fds是new[]出来的，析构时要注意delete[]
            }
        }

    private:
        uint16_t _port;  // 服务器端口号
        int _listenSockFd;  // 监听套接字
        struct pollfd* _poll_fds;
        func_t _func;  // 该函数用于对收到的数据进行业务逻辑处理
    };
}