#pragma once

#include <iostream>
#include <pthread.h>
#include <queue>
using namespace std;

const int _MaxCap = 5;

template<class T>
class BlockQueue
{
public:
    BlockQueue(const int& maxCapacity = _MaxCap)
        :_maxCapacity(maxCapacity)
    {
        // 初始化阻塞队列中的锁和两个条件变量
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_ccond, nullptr);
        pthread_cond_init(&_pcond, nullptr);
    }

    // 往阻塞队列中放置数据
    void push(const T& in)
    {
        // 申请锁，保护放置数据的安全性
        pthread_mutex_lock(&_mutex);

        // 1. 判断阻塞队列中是否满了
        while (isFull())
        {
            pthread_cond_wait(&_pcond, &_mutex);  // 阻塞队列为满，将生产者线程放到_pcond条件变量下的等待队列中等待
        }

        // 2. 代码执行到这，可以保证阻塞队列中一定没有满
        // 向阻塞队列中放置数据
        _q.push(in);

        // 3. 代码执行到这，可以保证阻塞队列中一定有数据，可以让消费者线程来拿数据
        // 唤醒_ccond条件变量下的等待队列中的消费者线程，来获取阻塞队列中的数据
        pthread_cond_signal(&_ccond);

        // 解锁
        pthread_mutex_unlock(&_mutex);
    }

    // 从阻塞队列中获取数据
    void pop(T* out)
    {
        // 申请锁，保护获取数据的安全性
        pthread_mutex_lock(&_mutex);

        // 1. 判断阻塞队列是否为空
        while (isEmpty())
        {
            pthread_cond_wait(&_ccond, &_mutex);  // 阻塞队列为空，将消费者线程放到_ccond条件变量下的等待队列中等待
        }

        // 2. 代码执行到这，可以保证阻塞队列一定不为空
        // 从阻塞队列中获取数据
        *out = _q.front();
        _q.pop();

        // 3. 代码执行到这，可以保证阻塞队列中一定有空位，可以让生产者线程来放数据
        // 唤醒_pcond条件变量下的等待队列中的生产者线程，来往阻塞队列中放置数据
        pthread_cond_signal(&_pcond);

        // 解锁
        pthread_mutex_unlock(&_mutex);
    }

    ~BlockQueue()
    {
        // 销毁阻塞队列中的锁和两个条件变量
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_ccond);
        pthread_cond_destroy(&_pcond);
    }

private:
    bool isFull()
    {
        return _q.size() == _maxCapacity;
    }

    bool isEmpty()
    {
        return _q.empty();
    }

private:
    queue<T> _q;
    int _maxCapacity;  // 表示队列中的元素上线
    pthread_mutex_t _mutex;  // 阻塞队列对于消费者线程和生产者线程来说是共享资源，需要被保护
    pthread_cond_t _ccond;  // 当队列为空，没有数据被消费，则让消费者线程到ccond条件变量的等待队列中挂起等待
    pthread_cond_t _pcond;  // 当队列为满，没有空间生产数据，则让生产者线程到pcond条件变量的等待队列中挂起等待
};

// C : calculate
// S : save
template<class C, class S>
class BlockQueues
{
public:
    BlockQueue<C> *c_bq;  // 计算任务的阻塞队列
    BlockQueue<S> *s_bq;  // 保存任务的阻塞队列
};