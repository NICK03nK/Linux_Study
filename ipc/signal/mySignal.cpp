#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <signal.h>
#include <cstdio>
#include <signal.h>
#include <unistd.h>
using namespace std;

int main()
{
    int cnt = 0;
    while (cnt < 10)
    {
        if (cnt == 5)
        {
            //raise(9);
            abort();  // 给当前进程发送6号信号
        }

        cout << cnt++ << endl;
        sleep(1);
    }
}

// // myKill的使用手册
// void Usage(const string& proc)
// {
//     cout << "Usage:" << endl;
//     cout << proc << " pid signo" << endl;
// }

// // 模拟一个kill命令
// int main(int argc, char* argv[])
// {
//     // 使用命令调用myKill的格式必须是 [./myKill pid signo] 所以argc要等于3
//     if (argc != 3)
//     {
//         Usage(argv[0]);
//         exit(1);
//     }

//     int pid = atoi(argv[1]);
//     int signo = atoi(argv[2]);

//     int ret = kill(pid, signo);  // signo可以传任意信号
//     if (ret == -1)
//     {
//         perror("kill");
//     }

//     return 0;
// }