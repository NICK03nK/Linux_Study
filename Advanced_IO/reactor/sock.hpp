#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string>

#include "log.hpp"
#include "err.hpp"

static const int backlog = 32;
static const int defaultSockFd = -1;

class Sock
{
public:
    Sock()
        :_listenSockFd(defaultSockFd)
    {}

    ~Sock()
    {
        if (_listenSockFd != defaultSockFd)
        {
            close(_listenSockFd);
        }
    }

public:
    int Socket() // 将创建套接字封装成一个方法
    {
        // 1.创建tcp套接字
        _listenSockFd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listenSockFd == defaultSockFd)
        {
            logMessage(FATAL, "create socket error");
            exit(SOCK_ERROR);
        }
        logMessage(NORMAL, "create socket success: %d", _listenSockFd);

        // 设置地址复用
        int opt = 1;
        setsockopt(_listenSockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    void Bind(uint16_t port) // 将套接字与ip和端口绑定封装成一个方法
    {
        // 2.调用bind()，将ip和port与套接字建立链接
        struct sockaddr_in local;
        bzero(&local, sizeof(local)); // 清空local
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY; // 服务器程序绑定默认ip(即0.0.0.0)

        int ret = bind(_listenSockFd, (struct sockaddr *)&local, sizeof(local));
        if (ret == -1)
        {
            logMessage(FATAL, "bind socket error");
            exit(BIND_ERROR);
        }
        logMessage(NORMAL, "bind socket success");
    }

    void Listen()
    {
        // 3.调用listen()，将套接字设置为监听状态
        int ret = listen(_listenSockFd, backlog);
        if (ret == -1)
        {
            logMessage(FATAL, "socket listen error");
            exit(LISTEN_ERROR);
        }
        logMessage(NORMAL, "socket listen success");
    }
    
    int Accept(std::string* client_ip, uint16_t* client_port, int* err)
    {
        // 4.调用accept()，获取新链接
        struct sockaddr_in client;
        socklen_t len = sizeof(client);

        int sockFd = accept(_listenSockFd, (struct sockaddr *)&client, &len);
        *err = errno;

        if (sockFd == -1)
        {
            // logMessage(ERROR, "accept error, next");
        }
        else
        {
            // logMessage(NORMAL, "accept a new link success, get a new sockFd: %d", sockFd);

            *client_ip = inet_ntoa(client.sin_addr);
            *client_port = ntohs(client.sin_port);
        }

        return sockFd;  // 返回新连接的文件描述符
    }

    int Fd()
    {
        return _listenSockFd;
    }

    void Close()
    {
        if (_listenSockFd != defaultSockFd)
        {
            close(_listenSockFd);
        }
    }

private:
    int _listenSockFd;
};