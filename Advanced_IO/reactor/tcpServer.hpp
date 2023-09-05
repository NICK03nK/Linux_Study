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
#include "err.hpp"
#include "Protocol.hpp"

namespace Server
{
    struct Connection;
    class tcpServer;

    static const uint16_t defaultPort = 8080;
    static const int num = 64;

    using func_t = std::function<void(Connection*)>;
    // using handler_t = std::function<void(std::string&)>;

    struct Connection
    {
        int _sockFd;
        std::string _inbuffer;  // 输入缓冲区
        std::string _outbuffer; // 输出缓冲区

        func_t _receiver; // 从_sockFd中读取
        func_t _sender;   // 向_sockFd中写入
        func_t _excepter; // 处理_sockFd在IO的时候出现的异常事件

        tcpServer* _tsPoint; // 用于main中calculate()中，对tcpServer中的成员函数进行回调

        uint64_t lastTime;

        Connection(int sockFd, tcpServer* tsPoint)
            :_sockFd(sockFd)
            , _tsPoint(tsPoint)
        {}

        void Register(func_t r, func_t s, func_t e)
        {
            _receiver = r;
            _sender = s;
            _excepter = e;
        }

        void Close()
        {
            close(_sockFd);
        }

        ~Connection()
        {}
    };

    class tcpServer
    {
    public:
        tcpServer(func_t service, uint16_t port = defaultPort)
            :_port(port)
            , _revs(nullptr)
            , _service(service)
        {}

        void InitServer()
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

        void EnableReadWrite(Connection* conn, bool readable, bool writeable)
        {
            uint32_t event = (readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0) | EPOLLET;
            _epoller.Control(conn->_sockFd, event, EPOLL_CTL_MOD);
        }

        // 事件派发器
        void Dispatcher()
        {
            int timeout = 1000;
            while (true)
            {
                Loop(timeout);

                // 遍历_connections，计算每一个连接已经有多长时间没有执行任务了
                
            }
        }

        ~tcpServer()
        {
            // 关闭_sock和_epoller中的文件描述符
            _sock.Close();
            _epoller.Close();

            if (_revs != nullptr)
            {
                delete[] _revs;
            }
        }

    private:
        void Receiver(Connection *conn)
        {
            conn->lastTime = time(nullptr);

            char buffer[1024] = {0};
            while (true)
            {
                ssize_t n = recv(conn->_sockFd, buffer, sizeof(buffer) - 1, 0);
                if (n > 0)
                {
                    buffer[n] = 0;
                    conn->_inbuffer += buffer; // 将读到的数据添加到对应文件描述符的输入缓冲区中
                    _service(conn);
                }
                else if (n == 0) // 对端关闭了连接
                {
                    if (conn->_excepter)
                    {
                        conn->_excepter(conn);
                        return;
                    }
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        break;
                    }
                    else if (errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        if (conn->_excepter)
                        {
                            conn->_excepter(conn);
                            return;
                        }
                    }
                }
            }
        }

        void Sender(Connection* conn)
        {
            conn->lastTime = time(nullptr);

            while (true)
            {
                ssize_t n = send(conn->_sockFd, conn->_outbuffer.c_str(), conn->_outbuffer.size(), 0);
                std::cout << "n: " << n << std::endl;
                if (n > 0)
                {
                    if (conn->_outbuffer.empty()) // 说明数据已发完
                    {
                        EnableReadWrite(conn, true, false);
                        break;
                    }
                    else
                    {
                        conn->_outbuffer.erase(0, n);
                    }
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        break;
                    }
                    else if (errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        if (conn->_excepter)
                        {
                            conn->_excepter(conn);
                            return;
                        }
                    }
                }
            }

            // 如果没有将数据发送完毕，则需要对对应的文件描述符开启对写事件的关心；如果数据发送完毕，则需要关闭对写事件的关心
            if (!conn->_outbuffer.empty())
            {
                conn->_tsPoint->EnableReadWrite(conn, true, true);
            }
            else
            {
                conn->_tsPoint->EnableReadWrite(conn, true, false);
            }
        }

        void Excepter(Connection* conn)
        {
            // 将对应的文件描述符从epoll模型中移除
            _epoller.Control(conn->_sockFd, 0, EPOLL_CTL_DEL);

            // 从epoll模型中移除文件描述符后，再关闭该文件描述符
            conn->Close();

            // 再将文件描述符对应的Connection对象从_connections中移除，并且调用delete释放为conn开辟的空间
            _connections.erase(conn->_sockFd);
            logMessage(DEBUG, "Close all sockFd: %d resources", conn->_sockFd);
            delete conn;
        }

        // 获取新连接
        void Accepter(Connection* conn)
        {
            while (true) // 循环获取新连接，直到Accept()返回值小于零时返回
            {
                std::string client_ip;
                uint16_t client_port;
                int err = 0;
                int sockFd = _sock.Accept(&client_ip, &client_port, &err);

                if (sockFd > 0)
                {
                    // 将对应套接字添加到unordered_map中
                    AddConnection(sockFd, EPOLLIN | EPOLLET,
                                  std::bind(&tcpServer::Receiver, this, std::placeholders::_1),
                                  std::bind(&tcpServer::Sender, this, std::placeholders::_1),
                                  std::bind(&tcpServer::Excepter, this, std::placeholders::_1));

                    logMessage(DEBUG, "get new link, info: [%s:%d]", client_ip.c_str(), client_port);
                }
                else if (sockFd == -1)
                {
                    if (err == EAGAIN || err == EWOULDBLOCK) // 文件描述符没有读取到任何数据
                    {
                        break;
                    }
                    else if (err == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        void AddConnection(int sockFd, uint32_t events, func_t receiver, func_t send, func_t excepter)
        {
            // 将epoll设置为ET模式前，要将对应的fd设置为非阻塞模式
            if (events & EPOLLET) // 将监听套接字设置为非阻塞
            {
                Util::setNoneBlock(sockFd);
            }

            // 1.为sockFd创建Connection对象
            Connection* conn = new Connection(sockFd, this);
            conn->Register(receiver, send, excepter); // 给对应的sockFd的Connection对象设置回调方法

            // 2.将sockFd添加到epoll模型中
            bool ret = _epoller.AddEvent(sockFd, events);
            assert(ret);
            (void)ret;

            // 3.将kv添加到_connections中
            _connections.insert(std::make_pair(sockFd, conn));

            logMessage(DEBUG, "add a new sockFd: %d in epoll and unordered_map", sockFd);
        }

        bool IsConnectionExists(int sockFd)
        {
            auto it = _connections.find(sockFd);
            return it != _connections.end();
        }

        void Loop(int timeout)
        {
            int ret = _epoller.Wait(_revs, _num, timeout); // 获取已经就绪的事件
            for (int i = 0; i < ret; ++i)
            {
                int sockFd = _revs[i].data.fd; // 已就绪的文件描述符
                uint32_t events = _revs[i].events;

                // 将所有的异常问题转交给读写来处理
                if (events & EPOLLERR)
                {
                    events |= (EPOLLIN | EPOLLOUT);
                }
                if (events & EPOLLHUP)
                {
                    events |= (EPOLLIN | EPOLLOUT);
                }

                if ((events & EPOLLIN) && IsConnectionExists(sockFd) && _connections[sockFd]->_receiver) // 文件描述符的读事件就绪，且connection对象在map中存在，且对应connection对象的_receiver()不为nullptr
                {
                    _connections[sockFd]->_receiver(_connections[sockFd]);
                }
                if ((events & EPOLLOUT) && IsConnectionExists(sockFd) && _connections[sockFd]->_sender)
                {
                    _connections[sockFd]->_sender(_connections[sockFd]);
                }
            }
        }

    private:
        uint16_t _port;
        Sock _sock;
        Epoller _epoller;
        std::unordered_map<int, Connection*> _connections;
        struct epoll_event* _revs;
        int _num;
        // handler_t _handler;
        func_t _service;
    };
}