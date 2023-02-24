#include <iostream>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

// 用于测试系统调用发送信号
int main()
{
    while (true)
    {
        cout << "我是一个正在运行的进程，pid: " << getpid() << endl;
        sleep(1);
    }

    return 0;
}