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

#include "protocol.hpp"

using namespace std;

namespace Server
{
    enum {USE_ERROR = 1, SOCK_ERROR, BIND_ERROR, LISTEN_ERROR};

    static const int backlog = 5;  //?

    using func_t = function<bool (const httpRequest&, const httpResponse&)>;

    class httpServer
    {
    public:
        httpServer(const uint16_t& port, func_t func)
            :_port(port)
            , _listenSockFd(-1)
            , _func(func)
        {}

        void initServer()
        {
            // 1.创建tcp套接字
            _listenSockFd = socket(AF_INET, SOCK_STREAM, 0);
            if (_listenSockFd == -1)
            {
                exit(SOCK_ERROR);
            }

            // 2.调用bind()，将ip和port与套接字建立链接
            struct sockaddr_in local;
            bzero(&local, sizeof(local));  // 清空local
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;  // 服务器程序绑定默认ip(即0.0.0.0)

            int ret = bind(_listenSockFd, (struct sockaddr*)&local, sizeof(local));
            if (ret == -1)
            {
                exit(BIND_ERROR);
            }

            // 3.调用listen()，将套接字设置为监听状态
            ret = listen(_listenSockFd, backlog);
            if (ret == -1)
            {
                exit(LISTEN_ERROR);
            }
        }

        void handlerHttp(int sockFd)
        {
            // 1.读取一个完整的http请求
            // 2.对请求进行反序列化
            // 3.调用_func(req, resp)，将请求转换成响应
            // 4.将resp序列化
            // 5.调用send()，将resp发送给对端

            char buffer[4096] = { 0 };
            httpRequest req;
            httpResponse resp;

            ssize_t n = recv(sockFd, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                req.inbuffer = buffer;
                _func(req, resp);  // 将请求转换成响应
                send(sockFd, resp.outbuffer.c_str(), resp.outbuffer.size(), 0);
            }
        }

        void start()
        {
            while (true)
            {
                // 4.调用accept()，获取新链接
                struct sockaddr_in client;
                socklen_t len = sizeof(client);

                int sockFd = accept(_listenSockFd, (struct sockaddr*)&client, &len);
                if (sockFd == -1)
                {
                    continue;
                }
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
                    handlerHttp(sockFd);

                    close(sockFd);
                    exit(0);
                }

                // 父进程
                waitpid(id, nullptr, 0);  // 阻塞式等待子进程

                close(sockFd);  // 父进程关闭sockFd，以免出现文件描述符泄漏的问题
                // --the end of version 2
            }
        }

        ~httpServer()
        {}

    private:
        uint16_t _port;  // 服务器程序的端口号
        int _listenSockFd;  // 用于监听是否有新链接，作为accpet()的参数
        func_t _func;
    };
}