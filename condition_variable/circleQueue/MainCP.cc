#include "CircleQueue.hpp"
#include "Task.hpp"
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <string>

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
        cout << "生产者生产了一个任务 >> " << t.toTaskString() << endl;
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
        cout << "消费者消费了一个任务 >> " << result << endl;
    }
}

int main()
{
    srand((unsigned int)time(nullptr) ^ getpid() ^ pthread_self());

    CircleQueue<Task>* cq = new CircleQueue<Task>();

    pthread_t producer, consumer;
    pthread_create(&producer, nullptr, produce, cq);
    pthread_create(&consumer, nullptr, consume, cq);

    pthread_join(producer, nullptr);
    pthread_join(consumer, nullptr);

    delete cq;

    return 0;
}