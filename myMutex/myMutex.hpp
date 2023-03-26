#pragma once

#include <iostream>
#include <pthread.h>
using namespace std;

namespace myMutex
{
    class mutex
    {
    public:
        mutex(pthread_mutex_t* p_mtx = nullptr)
            :_p_mtx(p_mtx)
        {}

        void lock()
        {
            if (_p_mtx)
            {
                pthread_mutex_lock(_p_mtx);
            }
        }

        void unlock()
        {
            if (_p_mtx)
            {
                pthread_mutex_unlock(_p_mtx);
            }
        }

        ~mutex()
        {}

    private:
        pthread_mutex_t* _p_mtx;
    };
}

// RAII风格的自动加锁解锁类
class LockGuard
    {
    public:
        LockGuard(pthread_mutex_t* p_mtx)
            :_mtx(p_mtx)
        {
            _mtx.lock();  // 在LockGuard对象构造时自动加锁
        }

        ~LockGuard()
        {
            _mtx.unlock();  // 在LockGuard对象析构时自动解锁
        }

    private:
        myMutex::mutex _mtx;
    };