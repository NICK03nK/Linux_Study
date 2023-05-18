#pragma once

#include "myThread.hpp"
#include "LockGuard.hpp"
#include <vector>
#include <queue>
#include <pthread.h>
#include <unistd.h>

#include "logMessage.hpp"

using namespace myThread;

static const size_t threadNum = 10;

// 对ThreadPool类做声明，以免ThreadData不认识
template<class T>
class ThreadPool;

template<class T>
class ThreadData
{
public:
    ThreadPool<T>* _pThreadPool;  // 指针，指向线程池
    string _threadName;  // 保存线程池中每个线程的名称

public:
    ThreadData(ThreadPool<T>* pThreadPool, const string& threadName)
        :_pThreadPool(pThreadPool)
        , _threadName(threadName)
    {}
};

template<class T>
class ThreadPool
{
public:
    ThreadPool(size_t num = threadNum)
        :_num(num)
    {
        // 初始化锁和条件变量
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);

        // 创建线程池中的一批线程，并将线程的指针push到vector中管理起来
        for (int i = 0; i < _num; ++i)
        {
            _threads.push_back(new thread());  // 将线程池对象的this指针作为handlerTask的参数传递，因为handlerTask是static成员函数没有this指针，将this指针作为参数进行传递，便于在handlerTask通过this来访问非static的成员变量(_taskQueue、_mutex、_cond)
        }
    }

    void run()
    {
        for (const auto& t : _threads)
        {
            ThreadData<T>* td = new ThreadData<T>(this, t->getThreadName());

            t->start(handlerTask, td);
            // cout << t->getThreadName() << "start..." << endl;
            logMessage(DEBUG, "%s start...", t->getThreadName().c_str());
        }
    }

    // 给外部提供一个向任务队列生产任务的接口
    void push(const T& in)
    {
        // pthread_mutex_lock(&_mutex);

        // 使用RAII的管理方式来加锁
        LockGuard lck(&_mutex);

        _taskQueue.push(in);
        pthread_cond_signal(&_cond);  // 向任务队列中push任务后，从条件变量的等待队列中唤醒一个线程来处理任务

        // pthread_mutex_unlock(&_mutex);
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);

        for (const auto& t : _threads)
        {
            delete t;  // _threads中保存的都是new出来的线程对象的指针，所以用delete来释放
        }
    }

public:
    // 申请互斥锁接口
    void lockQueue()
    {
        pthread_mutex_lock(&_mutex);
    }

    // 解锁接口
    void unlockQueue()
    {
        pthread_mutex_unlock(&_mutex);
    }

    // 判断任务队列是否为空接口
    bool isQueueEmpty()
    {
        return _taskQueue.empty();
    }

    // 将线程放到条件变量等待队列中挂起等待接口
    void threadWait()
    {
        pthread_cond_wait(&_cond, &_mutex);
    }

    T pop()
    {
        T t = _taskQueue.front();  // 从任务队列中获取任务
        _taskQueue.pop();

        return t;
    }

    pthread_mutex_t& getMutex()
    {
        return _mutex;
    }

private:
    // 由于handlerTask是static成员函数，对应的操作做了相关的封装
    static void* handlerTask(void* arg)
    {
        ThreadData<T>* td = static_cast<ThreadData<T>*>(arg);

        while (true)  // 此处使用while(true)的原因是，保持线程池中的线程，有任务就取任务并处理，没任务就在线程池中挂起等待
        {
            T t;
            {
                // 使用RAII的管理方式来加锁
                LockGuard lck(&(td->_pThreadPool->getMutex()));

                // 判断任务队列是否为空
                while (td->_pThreadPool->isQueueEmpty())
                {
                    td->_pThreadPool->threadWait();  // 任务队列为空，将线程挂起等待
                }

                // 线程从任务队列中获取任务
                t = td->_pThreadPool->pop();
            }

            t();  // 获取任务后，处理任务
        }
        delete td;  // td是在run中new出来的，使用完毕后要用delete释放

        return nullptr;
    }

private:
    size_t _num;  // 表示线程池中线程的个数
    vector<thread*> _threads;  // 用一个vector管理线程池中的一批线程
    queue<T> _taskQueue;  // 任务队列(对于线程池中的线程属于共享资源)，线程池的线程从该任务队列中获取任务并处理
    pthread_mutex_t _mutex;  // 互斥锁，用于保护_taskQueue在被多线程访问时的安全性
    pthread_cond_t _cond;  // 任务队列中没有任务就让线程池中的线程挂起等待，有任务就唤醒线程从任务队列中获取任务
};