#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <functional>

#include "logMessage.hpp"
#include "protocol.hpp"

using namespace std;

namespace Server
{
    enum {USE_ERROR = 1, SOCK_ERROR, BIND_ERROR, LISTEN_ERROR};

    static const int backlog = 5;  //?

    using func_t = function<bool(const Request&, Response&)>;

    void handlerEntery(int sockFd, func_t func)
    {
        string inbuffer;

        while (true)
        {
            // 1.读取一个完整的报文，并且对其解包，拿到有效载荷
            string recvPackage;
            if (!GetPackage(sockFd, inbuffer, &recvPackage))
            {
                return;  // 获取一个完整的报文失败，直接返回
            }

            string reqMessage = deLength(recvPackage);

            // 2.将该有效载荷反序列化处理，得到一个Request对象(结构化数据)
            Request req;
            if (!req.deserialize(reqMessage))
            {
                return;  // 反序列化失败，直接返回
            }

            // 3.执行计算器处理，并且得到一个Response对象(结构化数据) -- 对两个操作数做对应的运算判断(业务逻辑)
            Response resp;
            func(req, resp);        

            // 4.将Response对象做序列化处理
            string respMessage;
            resp.serialize(&respMessage);

            // 5.给响应添加报头封装成报文，发回给客户端
            string sendPackage = enLength(respMessage);
            send(sockFd, sendPackage.c_str(), sendPackage.size(), 0);
        }
    }

    class tcpServer
    {
    public:
        tcpServer(const uint16_t& port)
            :_port(port)
            , _listenSockFd(-1)
        {}

        void initServer()
        {
            // 1.创建tcp套接字
            _listenSockFd = socket(AF_INET, SOCK_STREAM, 0);
            if (_listenSockFd == -1)
            {
                logMessage(FATAL, "create socket error");
                exit(SOCK_ERROR);
            }
            logMessage(NORMAL, "create socket success");

            // 2.调用bind()，将ip和port与套接字建立链接
            struct sockaddr_in local;
            bzero(&local, sizeof(local));  // 清空local
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;  // 服务器程序绑定默认ip(即0.0.0.0)

            int ret = bind(_listenSockFd, (struct sockaddr*)&local, sizeof(local));
            if (ret == -1)
            {
                logMessage(FATAL, "bind socket error");
                exit(BIND_ERROR);
            }
            logMessage(NORMAL, "bind socket success");

            // 3.调用listen()，将套接字设置为监听状态
            ret = listen(_listenSockFd, backlog);
            if (ret == -1)
            {
                logMessage(FATAL, "socket listen error");
                exit(LISTEN_ERROR);
            }
            logMessage(NORMAL, "socket listen success");
        }

        void start(func_t func)
        {
            while (true)
            {
                // 4.调用accept()，获取新链接
                struct sockaddr_in client;
                socklen_t len = sizeof(client);

                int sockFd = accept(_listenSockFd, (struct sockaddr*)&client, &len);
                if (sockFd == -1)
                {
                    logMessage(ERROR, "accept error, next");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success, get a new sockFd: %d", sockFd);

                // 5.未来在进行tcp网络编程通信时，都是对accept()的返回值sockFd进行文件操作

                // --version 2 (多进程版本)
                pid_t id = fork();  // 创建子进程

                if (id == 0)  // 子进程
                {
                    close(_listenSockFd);  // 关闭用来监听的sockfd，避免对其误操作

                    if (fork() > 0)  // 在子进程中再创建子进程
                    {
                        // 当前为子进程(非孙子进程)
                        exit(0);  // 子进程退出了，孙子进程就会变成孤儿进程，由OS领养，当孙子进程退出时，由OS负责回收，即回收任务不需要父进程来关注
                    }

                    // 孙子进程
                    handlerEntery(sockFd, func);  // 由孙子进程执行业务逻辑
                    close(sockFd);
                    exit(0);
                }

                // 父进程
                pid_t ret = waitpid(id, nullptr, 0);  // 阻塞式等待子进程
                if (ret > 0)
                {
                    logMessage(NORMAL, "wait child success");
                }

                close(sockFd);  // 父进程关闭sockFd，以免出现文件描述符泄漏的问题
                // --the end of version 2
            }
        }

        ~tcpServer()
        {}

    private:
        uint16_t _port;  // 服务器程序的端口号
        int _listenSockFd;  // 用于监听是否有新链接，作为accpet()的参数
    };
}