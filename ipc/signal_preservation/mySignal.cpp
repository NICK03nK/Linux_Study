#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <vector>
using namespace std;

// #define BLOCK_SIGNAL 2
#define MAX_SIGNUM 31

// static vector<int> sigarr = { 2, 3 };  // 要屏蔽的信号
static vector<int> sigarr = { 2 };  // 要屏蔽的信号

static void show_pending(const sigset_t& pending)
{
    for (int signo = MAX_SIGNUM; signo >= 1; signo--)
    {
        if (sigismember(&pending, signo))
        {
            cout << "1";
        }
        else
        {
            cout << "0";
        }
    }
    cout << endl;
}

static sigset_t block, oblock, pending;  // sigset_t是信号集类型，定义的变量都是一个信号集

void myHandler(int signo)
{
    cout << "特定信号已递达" << endl;

    cout << "sleep-5" << endl;
    sleep(5);

    // BUG
    // cout << "再次屏蔽特殊信号" << endl;
    // sigprocmask(SIG_SETMASK, &block, &oblock);
}

int main()
{
    for (const auto& e : sigarr)
    {
        signal(e, myHandler);
    }

    // 1.尝试屏蔽特定的信号    
    // 1.1.初始化信号集
    sigemptyset(&block);
    sigemptyset(&oblock);
    sigemptyset(&pending);

    // 1.2.添加要屏蔽的信号
    for (const auto& e : sigarr)
    {
        sigaddset(&block, e);
    }

    // 1.3.屏蔽信号
    sigprocmask(SIG_SETMASK, &block, &oblock);  // 修改当前进程的信号屏蔽字(block表)

    // 2.遍历打印pending信号集
    int cnt = 5;
    while (true)
    {
        // 2.1.初始化
        sigemptyset(&pending);

        // 2.2.获取pending信号集
        sigpending(&pending);  // 覆盖式获取当前进程的未决信号集，放入到pending变量中

        // 2.3.打印信号集
        cout << cnt << endl;
        cout << "pending:";
        show_pending(pending);

        sleep(1);

        // 3.取消对特定信号的阻塞
        if (cnt-- == 0)
        {
            cout << "取消对特定信号的屏蔽" << endl;

            sigprocmask(SIG_SETMASK, &oblock, &block);  // 取消特定信号的阻塞状态

            // 将再次屏蔽特殊信号的代码放在此处可以正常工作
            cout << "再次屏蔽特殊信号" << endl;
            sigprocmask(SIG_SETMASK, &block, &oblock);

            cnt = 5;
        }
    }

    return 0;
}