#pragma once

#include <iostream>
#include <semaphore.h>
#include <vector>
using namespace std;

static const size_t MaxCap = 5;

template<class T>
class CircleQueue
{
public:
    CircleQueue(size_t maxCapacity = MaxCap)
        :_queue(maxCapacity)
        , _maxCapacity(maxCapacity)
    {
        sem_init(&_spaceSem, 0, _maxCapacity);
        sem_init(&_dataSem, 0, 0);

        _producerPos = _consumerPos = 0;
    }

    void push(const T& in)
    {
        // 生产者申请空间资源
        P(_spaceSem);

        _queue[_producerPos++] = in;  // 向队列中放置数据，放完数据后_producerPos后移
        _producerPos %= _maxCapacity;  // 让_producerPos像绕圈一样

        // 生产者放完数据后，数据资源++
        V(_dataSem);
    }

    void pop(T* out)
    {
        // 消费者申请数据资源
        P(_dataSem);

        *out = _queue[_consumerPos++];  // 从队列中获取数据，拿完数据后_consumerPos后移
        _consumerPos %= _maxCapacity;  // 让_producerPos像绕圈一样

        // 消费者获取完数据后，空间资源++
        V(_spaceSem);
    }

    ~CircleQueue()
    {
        sem_destroy(&_spaceSem);
        sem_destroy(&_dataSem);
    }

private:
    void P(sem_t& sem)
    {
        sem_wait(&sem);
    }

    void V(sem_t& sem)
    {
        sem_post(&sem);
    }

private:
    vector<T> _queue;  // 用vector模拟环形队列
    size_t _maxCapacity;  // 队列的容量
    sem_t _spaceSem;  // 生产者线程 --> 空间资源
    sem_t _dataSem;  // 消费者线程 --> 数据资源
    size_t _producerPos;  // 生产者在队列中放置数据的位置
    size_t _consumerPos;  // 消费者从队列中获取数据的位置
};