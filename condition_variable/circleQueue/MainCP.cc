#include "CircleQueue.hpp"
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>

void* produce(void* arg)
{
    CircleQueue<int>* cq = static_cast<CircleQueue<int>*>(arg);

    while (true)
    {
        int data = rand() % 10 + 1;
        cq->push(data);
        cout << "生产数据完成，数据是 -> " << data << endl;
    }
}

void* consume(void* arg)
{
    CircleQueue<int>* cq = static_cast<CircleQueue<int>*>(arg);

    while (true)
    {
        int data = 0;
        cq->pop(&data);
        cout << "消费数据完成，数据是 -> " << data << endl;
    }
}

int main()
{
    srand((unsigned int)time(nullptr) ^ getpid() ^ pthread_self());

    CircleQueue<int>* cq = new CircleQueue<int>();

    pthread_t producer, consumer;
    pthread_create(&producer, nullptr, produce, cq);
    pthread_create(&consumer, nullptr, consume, cq);

    pthread_join(producer, nullptr);
    pthread_join(consumer, nullptr);

    delete cq;

    return 0;
}