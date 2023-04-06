#pragma once

#include <iostream>
#include <semaphore.h>
#include <vector>
#include <pthread.h>
using namespace std;

static const size_t MaxCap = 10;

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

        pthread_mutex_init(&_pmutex, nullptr);
        pthread_mutex_init(&_cmutex, nullptr);
    }

    void push(const T& in)
    {
        // 生产者申请空间资源
        P(_spaceSem);

        // 申请锁，保证生产者间互斥
        pthread_mutex_lock(&_pmutex);

        _queue[_producerPos++] = in;  // 向队列中放置数据，放完数据后_producerPos后移
        _producerPos %= _maxCapacity;  // 让_producerPos像绕圈一样

        // 解锁
        pthread_mutex_unlock(&_pmutex);

        // 生产者放完数据后，数据资源++
        V(_dataSem);
    }

    void pop(T* out)
    {
        // 消费者申请数据资源
        P(_dataSem);

        // 申请锁，保证消费者间互斥
        pthread_mutex_lock(&_cmutex);

        *out = _queue[_consumerPos++];  // 从队列中获取数据，拿完数据后_consumerPos后移
        _consumerPos %= _maxCapacity;  // 让_producerPos像绕圈一样

        // 解锁
        pthread_mutex_unlock(&_cmutex);

        // 消费者获取完数据后，空间资源++
        V(_spaceSem);
    }

    ~CircleQueue()
    {
        sem_destroy(&_spaceSem);
        sem_destroy(&_dataSem);

        pthread_mutex_destroy(&_pmutex);
        pthread_mutex_destroy(&_cmutex);
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
    pthread_mutex_t _pmutex;  // 用于保证生产者之间互斥
    pthread_mutex_t _cmutex;  // 用于保证消费者之间互斥
};