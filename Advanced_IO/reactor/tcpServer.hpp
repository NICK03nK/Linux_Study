#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <cassert>
#include "Epoller.hpp"
#include "Util.hpp"
#include "sock.hpp"
#include "log.hpp"
#include "err.h"

namespace Server
{
    struct Connection;
    class tcpServer;

    static const uint16_t defaultPort = 8080;
    static const int num = 64;

    using func_t = std::function<void(Connection *)>;

    struct Connection
    {
        int _sockFd;
        std::string _inbuffer;  // 输入缓冲区
        std::string _outbuffer; // 输出缓冲区

        func_t _receiver; // 从_sockFd中读取
        func_t _sender;   // 向_sockFd中写入
        func_t _excepter; // 处理_sockFd在IO的时候出现的异常事件

        tcpServer* _tsPoint; // ??

        Connection(int sockFd)
            :_sockFd(sockFd)
            , _tsPoint(nullptr)
        {}

        void Register(func_t r, func_t s, func_t e)
        {
            _receiver = r;
            _sender = s;
            _excepter = e;
        }

        ~Connection()
        {}
    };

    class tcpServer
    {
    public:
        tcpServer(uint16_t port = defaultPort)
            : _port(port)
        {}

        void initServer()
        {
            // 1.创建socket
            _sock.Socket();
            _sock.Bind(_port);
            _sock.Listen();

            // 2.构建epoll模型
            _epoller.Create();

            // 3.将当前唯一的一个fd添加到epoll模型中，即监听套接字
            // 将监听套接字看作一个Connection
            AddConnection(_sock.Fd(), EPOLLIN | EPOLLET, std::bind(&tcpServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);
            _revs = new struct epoll_event[num];
            _num = num;
        }

        // 获取新连接
        void Accepter(Connection* conn)
        {}

        void AddConnection(int sockFd, uint32_t events, func_t receiver, func_t send, func_t excepter)
        {
            // 将epoll设置为ET模式前，要将对应的fd设置为非阻塞模式
            if (events & EPOLLET) // 将监听套接字设置为非阻塞
            {
                Util::setNoneBlock(sockFd);
            }

            // 1.为sockFd创建Connection对象
            Connection* conn = new Connection(sockFd);
            conn->Register(receiver, send, excepter); // 给对应的sockFd的Connection对象设置回调方法

            // 2.将sockFd添加到epoll模型中
            bool ret = _epoller.AddEvent(sockFd, events);
            assert(ret);
            (void)ret;

            // 3.将kv添加到_connections中
            _connections.insert(std::make_pair(sockFd, conn));
        }

        void Loop(int timeout)
        {
            int ret = _epoller.Wait(_revs, _num, timeout); // 获取已经就绪的事件
            for (int i = 0; i < ret; ++i)
            {
                
            }
        }

        // 事件派发器
        void Dispatcher()
        {
            while (true)
            {
                Loop(1000);
            }
        }

        ~tcpServer()
        {}

    private:
        uint16_t _port;
        Sock _sock;
        Epoller _epoller;
        std::unordered_map<int, Connection*> _connections;
        struct epoll_event* _revs;
        int _num;
    };
}