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

#include "logMessage.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"

using namespace std;

namespace Server
{
    enum {USE_ERROR = 1, SOCK_ERROR, BIND_ERROR, LISTEN_ERROR};

    static const int backlog = 5;  //?
    
    class tcpServer;  // 提前声明tcpServer类，好让ThreadData知道后面是存在这个类的
    struct ThreadData
    {
        tcpServer* _self;  // 指向tcpServer对象，用于解决多线程版本的startRoutine()的传参问题
        int _sockFd;

        ThreadData(tcpServer* self, int sockFd)
            :_self(self)
            , _sockFd(sockFd)
        {}
    };

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
            // 4.初始化线程池
            _threadPool = new ThreadPool<Task>();  // new出来的线程池要在tcpServer的析构函数中delete掉
            logMessage(NORMAL, "init thread pool success");
            _threadPool->run();  // 将线程池跑起来

            while (true)
            {
                // 5.调用accept()，获取新链接
                struct sockaddr_in client;
                socklen_t len = sizeof(client);

                int sockFd = accept(_listenSockFd, (struct sockaddr*)&client, &len);
                if (sockFd == -1)
                {
                    logMessage(ERROR, "accept error, next");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success, get a new sockFd: %d", sockFd);

                // 6.未来在进行tcp网络编程通信时，都是对accept()的返回值sockFd进行文件操作

                // --version 1
                // serviceIO(sockFd);
                // close(sockFd);  // 执行完serviceIO()后，一定要关闭对应的sockFd，不然会导致文件描述符泄漏
                // --the end of version 1

                // --version 2 (多进程版本)
                // pid_t id = fork();  // 创建子进程

                // if (id == 0)  // 子进程
                // {
                //     close(_listenSockFd);  // 关闭用来监听的sockfd，避免对其误操作

                //     if (fork() > 0)  // 在子进程中再创建子进程
                //     {
                //         // 当前为子进程(非孙子进程)
                //         exit(0);  // 子进程退出了，孙子进程就会变成孤儿进程，由OS领养，当孙子进程退出时，由OS负责回收，即回收任务不需要父进程来关注
                //     }

                //     // 孙子进程
                //     serviceIO(sockFd);  // 由孙子进程来提供服务
                //     close(sockFd);
                //     exit(0);
                // }

                // // 父进程
                // pid_t ret = waitpid(id, nullptr, 0);  // 阻塞式等待子进程
                // if (ret > 0)
                // {
                //     cout << "wait success: " << ret << endl;
                // }

                // close(sockFd);  // 父进程关闭sockFd，以免出现文件描述符泄漏的问题
                // --the end of version 2

                // --version 3 (多线程版本)
                // pthread_t pid;
                // ThreadData* td = new ThreadData(this, sockFd);
                // pthread_create(&pid, nullptr, startRoutine, td);
                // --the end of version 3

                // --version 4 (线程池版本)
                Task t(sockFd, serviceIO);  // 创建任务
                _threadPool->push(t);  // 将任务push到线程池中
                // --the end of version 4
            }
        }

        ~tcpServer()
        {
            delete _threadPool;
        }

    private:

        // static void* startRoutine(void* arg)
        // {
        //     pthread_detach(pthread_self());  // 分离线程，让主线程不用join从线程，从而达到并发的目的

        //     ThreadData* td = static_cast<ThreadData*>(arg);

        //     td->_self->serviceIO(td->_sockFd);
        //     close(td->_sockFd);  // 使用完毕后，及时关闭文件描述符，避免文件描述符泄漏

        //     delete td;  // 释放new出来的td
            
        //     return nullptr;
        // }

    private:
        uint16_t _port;  // 服务器程序的端口号
        int _listenSockFd;  // 用于监听是否有新链接，作为accpet()的参数

        ThreadPool<Task>* _threadPool;  // 用于处理tcp网络通信的线程池版本
    };
}
