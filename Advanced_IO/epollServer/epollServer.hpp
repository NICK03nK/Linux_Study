#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include "err.hpp"
#include "log.hpp"
#include "sock.hpp"

namespace Server
{
    static const uint16_t defaultPort = 8080;
    static const int size = 128;
    static const int defaultValue = -1;
    static const int defaultNum = 64;

    using func_t = std::function<std::string(const std::string&)>;

    class epollServer
    {
    public:
        epollServer(func_t func, uint16_t port = defaultPort, int num = defaultNum)
            :_port(port)
            , _listenSockFd(defaultValue)
            , _epollFd(defaultValue)
            , _revs(nullptr)
            , _num(num)
            , _func(func)
        {}

        void initServer()
        {
            // 1.创建socket
            _listenSockFd = Sock::Socket();
            Sock::Bind(_listenSockFd, _port);
            Sock::Listen(_listenSockFd);

            // 2.创建epoll模型
            _epollFd = epoll_create(size);
            if (_epollFd == -1)
            {
                logMessage(FATAL, "epoll create error: %s", strerror(errno));
                exit(EPOLL_CREATE_ERROR);
            }

            // 3.将_listenSockFd添加到epoll中
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = _listenSockFd;  // 当事件就绪，被重新捞取上来的时候，需要知道是哪一个fd就绪了

            epoll_ctl(_epollFd, EPOLL_CTL_ADD, _listenSockFd, &ev);

            // 4.申请就绪队列的空间
            _revs = new struct epoll_event[_num];

            logMessage(NORMAL, "init server success!");
        }

        void HandlerEvent(int readyNum)
        {
            logMessage(DEBUG, "HandlerEvent in");

            for (int i = 0; i < readyNum; ++i)
            {
                uint32_t event = _revs[i].events;
                int sockFd = _revs[i].data.fd;

                if (sockFd == _listenSockFd && (event & EPOLLIN))
                {
                    // 监听套接字的读事件已就绪，即有新连接到来
                    // 获取新连接

                    std::string client_ip;
                    uint16_t client_port;
                    int sockFd = Sock::Accept(_listenSockFd, &client_ip, &client_port);
                    if (sockFd == -1)
                    {
                        logMessage(WARNING, "accept error");
                        continue;
                    }

                    // 获取到新连接，不能直接对fd进行读写操作，要先将获取的fd放入epoll模型中
                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = sockFd;
                    epoll_ctl(_epollFd, EPOLL_CTL_ADD, sockFd, &ev);                    
                }
                else if (event & EPOLLIN)
                {
                    // 普通IO读事件就绪
                    char buffer[1024] = { 0 };
                    ssize_t n = recv(sockFd, buffer, sizeof(buffer) - 1, 0);
                    if (n > 0)
                    {
                        buffer[n] = 0;
                        logMessage(NORMAL, "client# %s", buffer);

                        std::string response = _func(buffer);

                        send(sockFd, response.c_str(), response.size(), 0);  // 将处理完的buffer发送回给客户端
                    }
                    else if (n == 0)
                    {
                        logMessage(NORMAL, "client quit");

                        // 先将sockFd从epoll中移除，再close(sockFd)，因为epoll_ctl()要对有效的sockFd进行相应的操作
                        epoll_ctl(_epollFd, EPOLL_CTL_DEL, sockFd, nullptr);

                        close(sockFd);

                        return;
                    }
                    else
                    {
                        logMessage(ERROR, "recv() error, code: %d, err string: %s", errno, strerror(errno));

                        epoll_ctl(_epollFd, EPOLL_CTL_DEL, sockFd, nullptr);  // 将sockFd从epoll中移除

                        close(sockFd);
                    }
                }
                else
                {}
            }

            logMessage(DEBUG, "HandlerEvent out");
        }

        void start()
        {
            int timeout = -1;
            while (true)
            {
                int ret = epoll_wait(_epollFd, _revs, _num, timeout);
                switch (ret)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "epoll_wait() error, code: %d, err string: %s", errno, strerror(errno));
                    break;
                default:
                    // 有事件就绪了
                    logMessage(NORMAL, "have an event ready");

                    HandlerEvent(ret);  // 有ret个fd就绪了

                    break;
                }
            }
        }

        ~epollServer()
        {
            if (_listenSockFd != defaultValue)
            {
                close(_listenSockFd);
            }

            if (_epollFd != defaultValue)
            {
                close(_epollFd);
            }

            if (_revs != nullptr)
            {
                delete[] _revs;
            }
        }

    private:
        uint16_t _port;
        int _listenSockFd;
        int _epollFd;
        struct epoll_event* _revs;
        int _num;
        func_t _func;
    };
}