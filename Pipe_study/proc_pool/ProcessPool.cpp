#include <iostream>
#include <unistd.h>
#include <cassert>
#include <cstdlib>
using namespace std;

#define PROCESS_NUM 5

// 父进程控制子进程，子进程执行特定任务
int main()
{
    // 创建子进程并且建立和子进程的通信的信道
    for (int i = 0; i < PROCESS_NUM; ++i)
    {
        // 创建通信管道
        int fds[2] = { 0 };
        int n = pipe(fds);
        assert(n == 0);
        (void)n;

        // 创建子进程
        pid_t id = fork();
        if (id == 0)  // 子进程
        {
            close(fds[1]);

            // 子进程执行任务


            exit(0);
        }

        // 父进程，控制子进程


    }

    return 0;
}