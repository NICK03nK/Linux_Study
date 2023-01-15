#include <iostream>
#include <unistd.h>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

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
        // 子进程的通信代码

        exit(0);
    }

    // 父进程
    n = waitpid(id, nullptr, 0);
    assert(n == id);

    return 0;
}
