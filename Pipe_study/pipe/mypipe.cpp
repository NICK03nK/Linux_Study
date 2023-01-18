#include <iostream>
#include <unistd.h>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
using namespace std;

// 父进程读取，子进程写入
int main()
{
    // 第一步：创建管道文件，打开读写端
    int fds[2] = { 0 };
    int n = pipe(fds);
    assert(n == 0);

    // 第二步：fork()
    int id = fork();
    assert(id >= 0);

    // 子进程
    if (id == 0)
    {
        // 子进程进行写入
        close(fds[0]);
        // 子进程的通信代码
        const char *msg = "我是子进程，正在给父进程发消息";
        int cnt = 0;

        while (true)
        {
            ++cnt;
            char buffer[1024] = { 0 };  // 该缓冲区只有子进程能看见
            snprintf(buffer, sizeof(buffer), "child->parent:%s[%d][%d]", msg, cnt, getpid());
            write(fds[1], buffer, strlen(buffer));
            sleep(1);  // 细节：子进程每隔1s给父进程发送一次消息
        }

        close(fds[1]);
        exit(0);
    }
    
    // 父进程
    // 父进程进行读取
    close(fds[1]);

    // 父进程的通信代码
    while (true)
    {
        char buffer[1024] = { 0 };
        ssize_t s = read(fds[0], buffer, sizeof(buffer) - 1);  // 预留一个位置在buffer中写'\0'
        if (s > 0)
        {
            buffer[s] = '\0';  // s是在文件中读取的字符个数，在buffer[s]加上'\0'
        }

        cout << "Get Message >> " << buffer << " | mypid:" << getpid() << endl;  // 输出父进程读取到的消息
        // 细节：父进程没有进行sleep
    }

    n = waitpid(id, nullptr, 0);
    assert(n == id);

    close(fds[0]);

    return 0;
}
