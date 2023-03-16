#include <iostream>
#include <signal.h>
#include <unistd.h>
using namespace std;

#define BLOCK_SIG 2
#define MAX_SIGNUM 31

void show_pending(const sigset_t& pending)
{
    for (int i = MAX_SIGNUM; i >= 1; --i)
    {
        if (sigismember(&pending, i))
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

sigset_t block, oblock, pending;

void myHandler(int signo)
{
    cout << "BLOCK_SIG已递达" << endl;

    // 在myHandler中阻塞3号信号
    cout << "在myHandler中阻塞3号信号" << endl;

    // 将3号信号填入block中
    sigaddset(&block, 3);

    // 将添加了3号信号的block设置进当前进程的信号屏蔽字中
    sigprocmask(SIG_SETMASK, &block, &oblock);

    int cnt = 5;
    while (cnt--)
    {
        sleep(1);

        sigemptyset(&pending);

        // 获取当前进程的未决信号集并填入创建的pending信号集变量中
        sigpending(&pending);
        
        // 打印pending信号集
        cout << "pendng >> ";
        show_pending(pending);
    }
}

int main()
{
    signal(BLOCK_SIG, myHandler);

    // 初始化block，oblock信号集
    sigemptyset(&block);
    sigemptyset(&oblock);
    sigemptyset(&pending);

    // 将要阻塞的信号填入block中
    sigaddset(&block, BLOCK_SIG);

    // 将block设置进当前进程的信号屏蔽字中
    sigprocmask(SIG_SETMASK, &block, &oblock);
    cout << "阻塞BLOCK_NUM" << endl;

    int cnt = 5;
    while (true)
    {
        // 初始化pending信号集
        sigemptyset(&pending);

        // 获取当前进程的未决信号集并填入创建的pending信号集变量中
        sigpending(&pending);
        
        // 打印pending信号集
        cout << cnt-- << endl;
        cout << "pendng >> ";
        show_pending(pending);
        sleep(1);

        // 倒数5s后解除对BLOCK_SIG的阻塞
        if (cnt == 0)
        {
            cout << "解除对BLOCK_SIG的阻塞" << endl;
            sigprocmask(SIG_SETMASK, &oblock, &block);  // 解除对BLOCK_SIG的阻塞
        }
    }

    return 0;
}

// 程序解释：
// 先阻塞BLOCK_SIG，在5s内在终端窗口中ctrl+c发送2号信号，2号信号
// 被保存在pending中，5s后解除对BLOCK_SIG的阻塞，BLOCK_SIG被递
// 达，进入到myHandler方法中，正在处理BLOCK_SIG时，OS默认阻塞
// BLOCK_SIG，此时再次ctrl+c会看到pending中的第2个比特位由0置1
// 在myHandler方法中阻塞3号信号，在命令行中ctrl+\给进程发送3号
// 信号，发现pending中的第3个比特位由0置1，5s过后执行完myHandler
// 方法，OS会取消先前阻塞的2号信号和3号信号的阻塞状态，若检测到
// pending中有3号信号，则程序会直接Quit；若pending中只有2号信号
// ，则会再次执行myHandler。