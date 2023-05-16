#pragma once

#include "logMessage.hpp"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

namespace Server
{
    enum {USE_ERROR = 1, SOCK_ERROR, BIND_ERROR, LISTEN_ERROR};

    static const int backlog = 5;  //?

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
                    logMessage(ERROR, "accept error, next");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success");

                // 5.未来在进行tcp网络编程通信时，都是对accept()的返回值sockFd进行文件操作

                // --version 1
                // serviceIO(sockFd);
                // close(sockFd);  // 执行完serviceIO()后，一定要关闭对应的sockFd，不然会导致文件描述符泄漏
                // --end of version 1

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
                    serviceIO(sockFd);  // 由孙子进程来提供服务
                    close(sockFd);
                    exit(0);
                }

                // 父进程
                pid_t ret = waitpid(id, nullptr, 0);
                if (ret > 0)
                {
                    cout << "wait success: " << ret << endl;
                }
                
                close(sockFd);  // 父进程关闭sockFd，以免出现文件描述符泄漏的问题
            }
        }

        ~tcpServer()
        {}

    private:
        void serviceIO(int sockFd)
        {
            char buffer[1024] = { 0 };
            while (true)
            {
                ssize_t n = read(sockFd, buffer, sizeof(buffer) - 1);
                if (n > 0)
                {
                    buffer[n] = 0;
                    cout << "receive message: " << buffer << endl;

                    // 将服务器收到的消息转发回给客户端
                    string outbuffer = buffer;
                    outbuffer += "[server]";

                    write(sockFd, outbuffer.c_str(), outbuffer.size());
                }
                else if (n == 0)  // read()返回值为0，即client端已退出
                {
                    logMessage(NORMAL, "client quit, so do i");
                    break;
                }
            }
        }

    private:
        uint16_t _port;  // 服务器程序的端口号
        int _listenSockFd;  // 用于监听是否有新链接，作为accpet()的参数
    };
}
