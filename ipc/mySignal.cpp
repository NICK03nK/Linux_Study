#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

void handler(int signo)
{
    cout << "捕捉到了一个信号，信号编号是：" << signo << endl;
}

int main()
{
    signal(2, handler);

    while (true)
    {
        cout << "我是一个进程，id>> " << getpid() << endl;
        sleep(1);
    }

    return 0;
}