#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <cstring>
using namespace std;

string transformId(const pthread_t& threadId)
{
    char tid[128] = { 0 };
    snprintf(tid, sizeof(tid), "0x%x", threadId);
    return tid;
}

void* start_routine(void* args)
{
    string threadName = static_cast<const char*>(args);

    int cnt = 5;
    while (cnt--)
    {
        cout << "tid: " << transformId(pthread_self()) << " " << threadName << " is running..." << endl;
        sleep(1);
    }
}

int main()
{
    pthread_t tid = 0;
    pthread_create(&tid, nullptr, start_routine, (void*)"thread one");

    // 设置新线程为分离状态
    pthread_detach(pthread_self());

    string mainId = transformId(pthread_self());

    // 将新线程设置为分离状态后，主线程不用再阻塞式等待新线程
    // int n = pthread_join(tid, nullptr);
    // cout << "result:" << n << " : " << strerror(errno) << endl;

    while (true)
    {
        cout << "main thread is running... mainThreadId: " << mainId << " new thread id: " << transformId(tid) << endl;
        sleep(1);
    }

    return 0;
}