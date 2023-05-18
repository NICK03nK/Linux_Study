#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <cstdio>
#include <unistd.h>

#include "logMessage.hpp"

using namespace std;

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

    close(sockFd);  // 处理完任务，关闭文件描述符
}

class Task
{
    using func_t = function<void(int)>;
public:
    Task()
    {}

    Task(int sockFd, func_t callBack)
        :_sockFd(sockFd)
        , _callBack(callBack)
    {}

    void operator()()
    {
        _callBack(_sockFd);
    }

private:
    int _sockFd;
    func_t _callBack;
};