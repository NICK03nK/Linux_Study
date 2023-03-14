#include <iostream>
#include <pthread.h>
#include <cassert>
#include <unistd.h>
using namespace std;

// 新线程执行的函数
void* thread_routine(void* arg)
{
    const char* name = (const char*)arg;
    while (true)
    {
        cout << "我是新线程，正在运行！name >> " << name << endl;
        sleep(1);
    }
}

int main()
{
    pthread_t tid;

    // 创建新线程
    int n = pthread_create(&tid, nullptr, thread_routine, (void*)"thread one");
    assert(n == 0);
    (void)n;

    // 主线程
    while (true)
    {
        cout << "我是主线程，正在运行！" << endl;
        sleep(1);
    }

    return 0;
}