#include <iostream>
#include <pthread.h>
#include <cassert>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <vector>
#include <cassert>
using namespace std;

#define NUM 10

class ThreadData
{
public:
    long long _number;
    pthread_t _tid;
    char _namebuffer[64];
};

class ThreadReturn
{
public:
    int exitCode;
    int exitResult;
};

void* start_routine(void* args)
{
    sleep(1);

    ThreadData* td = static_cast<ThreadData*>(args);  // 使用static_cast来将同类型的变量安全的强制类型转换

    int cnt = 5;  // 每个线程循环打印5次就会结束
    while (cnt)
    {
        // 在多线程的情况下，每个线程有自己的私有栈空间，打印出来的cnt地址是不一样的
        cout << "new thread create success, name: " << td->_namebuffer << " cnt: " << cnt << " &cnt: " << &cnt << endl;
        cnt--;

        sleep(1);
    }

    // 线程退出
    // pthread_exit(nullptr);

    // return (void*)td->_number;

    // 线程退出返回一个对象
    // ThreadReturn* tr = new ThreadReturn();
    // tr->exitCode = 1;
    // tr->exitResult = 2;
    // return (void*)tr;

    // 线程正常退出返回100
    return (void*)100;
}

int main()
{
    // 1.创建一批进程
    vector<ThreadData*> threads;  // 存放创建的线程信息，用于管理线程

    for (int i = 0; i < NUM; ++i)
    {
        ThreadData* td = new ThreadData();
        
        td->_number = i + 1;
        snprintf(td->_namebuffer, sizeof(td->_namebuffer), "%s:%d", "thread", i + 1);
        pthread_create(&td->_tid, nullptr, start_routine, td);

        threads.push_back(td);

        // 这样给不同线程设置名字会因为缓冲区被覆盖而导致线程名字没有按照预期来设置
        // pthread_t tid;
        // char namebuffer[64] = { 0 };
        // snprintf(namebuffer, sizeof(namebuffer), "%s:%d", "thread", i + 1);
        // pthread_create(&tid, nullptr, start_routine, namebuffer);
        // sleep(1);
    }

    for (auto& td : threads)
    {
        cout << "create " << td->_namebuffer << " : " << td->_tid << " success" << endl;
    }

    sleep(2);

    // 取消掉前5个从线程
    for (int i = 0; i < threads.size() / 2; ++i)
    {
        pthread_cancel(threads[i]->_tid);
        cout << "cancel " << threads[i]->_namebuffer << " success" << endl;
    }
    // 线程被取消后，立马就会被join

    // 等待从线程
    for (auto& td : threads)
    {
        // void* ret = nullptr;  // 用于接收线程退出后的信息

        // int n = pthread_join(td->_tid, &ret);
        // assert(n == 0);

        // cout << "join " << td->_namebuffer << " success, ret: " << (long long)ret << endl;

        // // 进程被回收，释放保存线程属性的对象
        // delete td;

        // ThreadReturn* ret = nullptr;  // 用于接收线程退出后的信息

        // int n = pthread_join(td->_tid, (void**)&ret);
        // assert(n == 0);

        // cout << "join " << td->_namebuffer << " success, exitCode: " << ret->exitCode << " exitResult " << ret->exitResult << endl;

        void* ret = nullptr;
        int n = pthread_join(td->_tid, &ret);
        assert(n == 0);

        cout << "join " << td->_number << " success, exitCode: " << (long long)ret << endl;

        // 进程被回收，释放保存线程属性的对象
        delete td;

    }

    cout << "main thread quit" << endl;

    // while (true)
    // {
    //     cout << "new thread create success, name: main thread" << endl;
    //     sleep(1);
    // }

    return 0;
}






///////////////////////////////////////////////////////////////////////////////

// string func()
// {
//     return "我是一个独立的方法";
// }

// // 新线程执行的函数
// void* thread_routine(void* arg)
// {
//     const char* name = (const char*)arg;
//     while (true)
//     {
//         cout << "我是新线程，正在运行！name >> " << name << " : " << func() << endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     pthread_t tid;

//     // 创建新线程
//     int n = pthread_create(&tid, nullptr, thread_routine, (void*)"thread one");
//     assert(n == 0);
//     (void)n;

//     // 主线程
//     while (true)
//     {
//         char tidbuffer[64] = { 0 };
//         snprintf(tidbuffer, sizeof(tidbuffer), "0x%x", tid);

//         cout << "我是主线程，正在运行！创建出来的线程的tid：" << tidbuffer << " : " << func() << endl;
//         sleep(1);
//     }

//     return 0;
// }