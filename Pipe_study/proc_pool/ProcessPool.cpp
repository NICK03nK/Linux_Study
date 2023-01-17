#include <iostream>
#include <unistd.h>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstdio>
#include <ctime>
using namespace std;

#define PROCESS_NUM 5
#define MAKE_RAND() srand((unsigned long)time(nullptr) ^ getpid() ^ 0x11223344 ^ rand() % 1234)

typedef void(*func_t)();  // 函数指针 类型

void downLoadTask()
{
    cout << getpid() << ": downLoadTask\n" << endl;
    sleep(1);
}

void ioTask()
{
    cout << getpid() << ": ioTask\n" << endl;
    sleep(1);
}

void flushTask()
{
    cout << getpid() << ": flushTask\n" << endl;
    sleep(1);
}

// 子进程类
class subEP
{
public:
    subEP(pid_t subId, int writeFd)
        :_subId(subId)
        ,_writeFd(writeFd)
    {
        char nameBuffer[1024] = { 0 };
        snprintf(nameBuffer, sizeof(nameBuffer), "process-%d[pid(%d)-fd(%d)]", num++, _subId, _writeFd);
        _name = nameBuffer;
    }

public:
    static int num;
    string _name;  // 子进程的名字
    pid_t _subId;  // 子进程的pid
    int _writeFd;  // 子进程对应管道的写端的fd
};
int subEP::num = 0;

int receiveTask(int readFd)
{
    int code = 0;
    ssize_t n = read(readFd, &code, sizeof(code));
    assert(n == sizeof(code));

    return code;
}

void createSubProcess(vector<subEP>& subs, const vector<func_t>& funcMap)  // 创建PROCESS_NUM个子进程，并push到vector中
{
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
            while (true)
            {
                // 1. 获取父进程传来的任务码
                int taskCode = receiveTask(fds[0]);

                // 2. 执行任务
                if (taskCode >= 0 && taskCode < funcMap.size())
                {
                    funcMap[taskCode]();
                }
                else
                {
                    cout << "subprocess receive task error" << endl;
                }
            }

            exit(0);
        }

        // 父进程
        close(fds[0]);

        subEP sub(id, fds[1]);  // 创建一个子进程对象用于描述子进程的属性信息
        subs.push_back(sub);  // 将子进程对象放入vector中，便于统一管理
    }
}

void loadTaskFunc(vector<func_t>& funcMap)  // 将子进程要执行的任务push到vector中
{
    funcMap.push_back(downLoadTask);
    funcMap.push_back(ioTask);
    funcMap.push_back(flushTask);
}

void sendTask(const subEP& process, int taskIndex)
{
    cout << "task:" << taskIndex << " send to -> " << process._name << endl;
    ssize_t n = write(process._writeFd, &taskIndex, sizeof(taskIndex));
    assert(n == sizeof(taskIndex));
    (void)n;
}

void loadBalanceControl(const vector<subEP>& subs, const vector<func_t>& funcMap, int cnt)
{
    int processNum = subs.size();  // 子进程个数
    int taskNum = funcMap.size();  // 任务数
    bool forever = (cnt == 0 ? true : false);

    while (true)
    {
        // 1. 指定一个子进程
        int subIndex = rand() % processNum;

        // 2. 选择一个任务
        int taskIndex = rand() % taskNum;

        // 3. 将任务发送给选择的子进程
        sendTask(subs[subIndex], taskIndex);

        sleep(1);

        if (!forever)
        {
            --cnt;
            if (cnt == 0)
            {
                break;
            }
        }
    }

    // 关闭写端后，读端读到0后就自动退出读端
    
}

// 父进程控制子进程，子进程执行特定任务
int main()
{
    MAKE_RAND();
    // 1. 创建子进程并且建立和子进程的通信的信道
    vector<func_t> funcMap;
    loadTaskFunc(funcMap);
    vector<subEP> subs;
    createSubProcess(subs, funcMap);

    // 2. 父进程控制子进程
    int taskCnt = 3;  // 父进程发放任务的次数，若为0则永远发放任务
    loadBalanceControl(subs, funcMap, taskCnt);

    return 0;
}
