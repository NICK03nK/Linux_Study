#include "CircleQueue.hpp"
#include "Task.hpp"
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <string>

string SelfName()
{
    char name[128] = { 0 };
    snprintf(name, sizeof(name), "thread[0x%x]", pthread_self());

    return name;
}

void* produce(void* arg)
{
    CircleQueue<Task>* cq = static_cast<CircleQueue<Task>*>(arg);

    while (true)
    {
        // 构建任务
        int x = rand() % 30 + 1;
        int y = rand() % 15 + 1;
        char option = operation[rand() % operation.size()];
        Task t(x, y, option, myMath);

        // 生产任务
        cq->push(t);
        cout << SelfName() << "生产者生产了一个任务 >> " << t.toTaskString() << endl;
    }
}

void* consume(void* arg)
{
    CircleQueue<Task>* cq = static_cast<CircleQueue<Task>*>(arg);

    while (true)
    {
        // 消费任务
        Task t;
        cq->pop(&t);
        string result = t();
        cout << SelfName() << "消费者消费了一个任务 >> " << result << endl;
    }
}

int main()
{
    srand((unsigned int)time(nullptr) ^ getpid() ^ pthread_self());

    CircleQueue<Task>* cq = new CircleQueue<Task>();

    pthread_t producer[4], consumer[8];

    for (int i = 0; i < 4; ++i)
    {
        pthread_create(producer + i, nullptr, produce, cq);
    }
    for (int i = 0; i < 8; ++i)
    {
        pthread_create(consumer + i, nullptr, consume, cq);
    }

    for (int i = 0; i < 4; ++i)
    {
        pthread_join(producer[i], nullptr);
    }
    for (int i = 0; i < 8; ++i)
    {
        pthread_join(consumer[i], nullptr);
    }

    delete cq;

    return 0;
}