#pragma once

#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEV "/dev/null"

void daemonSelf(const char* currPath = nullptr)
{
    // 1.让调用该守护进程化函数的进程忽略掉SIGPIPE型号(防止client退出导致服务器退出)
    signal(SIGPIPE, SIG_IGN);

    // 2.解决调用daemonSelf()的进程是组长的问题
    if (fork() > 0)  // 该进程是父进程，即组长
    {
        exit(0);  // 退出父进程
    }
    // 子进程
    pid_t ret = setsid();
    assert(ret != -1);

    // 3.守护进程是脱离终端的，重定向以前进程默认打开的文件
    int fd = open(DEV, O_RDWR);
    if (fd >= 0)
    {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);

        close(fd);
    }
    else  // open()调用失败
    {
        close(0);
        close(1);
        close(2);
    }

    // 4.可选：进程执行路径发生改变
    if (currPath)
    {
        chdir(currPath);
    }
}