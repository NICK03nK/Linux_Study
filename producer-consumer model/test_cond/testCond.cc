#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <string>
#include <unistd.h>
using namespace std;

int tickets = 1000;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

void* start_routine(void* args)
{
    string name = static_cast<const char*>(args);

    // 抢票逻辑
    while (true)
    {
        // 先申请锁，保证临界资源的安全性
        pthread_mutex_lock(&mtx);
        pthread_cond_wait(&cnd, &mtx);  // 先直接让线程去条件变量那等待

        cout << name << " -> " << tickets << endl;
        tickets--;

        pthread_mutex_unlock(&mtx);
    }
}

int main()
{
    // 创建5个线程
    pthread_t threads[5] = { 0 };
    for (int i = 0; i < 5; ++i)
    {
        char* namebuffer = new char[64];
        snprintf(namebuffer, 64, "thread -> %d", i + 1);
        pthread_create(threads + i, nullptr, start_routine, namebuffer);
    }

    while (true)
    {
        sleep(1);

        // 每间隔1s从条件变量那唤醒一个线程去继续执行start_routine
        pthread_cond_signal(&cnd);
        cout << "main thread wakeup a thread..." << endl;
    }

    // 回收所有线程
    for (int i = 0; i < 5; ++i)
    {
        pthread_join(threads[i], nullptr);
    }

    return 0;
}