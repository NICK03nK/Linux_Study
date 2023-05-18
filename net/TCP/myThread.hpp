#pragma once

#include <iostream>
#include <pthread.h>
#include <string>
#include <functional>
#include <cstdio>
#include <cassert>

using namespace std;

namespace myThread
{
    using func_t = function<void*(void*)>;
    const int num = 1024;

    class thread
    {
    public:
        thread()
        {
            // 初始化线程名称
            char namebuffer[num];
            snprintf(namebuffer, sizeof(namebuffer), "thread-%d", _threadNum++);  // 此处_threadNum++不会出现线程不安全的情况，因为只在主线程中创建thread对象，只有一个执行流执行，不会出现多个线程同时创建thread对象的情况
            _name = namebuffer;
        }

        void start(func_t func, void* arg = nullptr)
        {
            _func = func;
            _arg = arg;
            
            int n = pthread_create(&_tid, nullptr, start_routine, this);  // 将当前线程对象的this指针作为参数传给start_routine
            assert(n == 0);
            (void)n;
        }

        void join()
        {
            int n = pthread_join(_tid, nullptr);
            assert(n == 0);
            (void)n;
        }

        string getThreadName()
        {
            return _name;
        }

        void* callBack()
        {
            return _func(_arg);  // 执行线程要调用的函数
        }

        ~thread()
        {}
    
    private:
        static void* start_routine(void* arg)
        {
            thread* _this = static_cast<thread*>(arg);
            return _this->callBack();  // 通过当前线程对象的this指针调用callBack函数
        }

    private:
        string _name;  // 线程名称
        func_t _func;  // 线程执行函数
        void* _arg;  // 线程执行函数的参数
        pthread_t _tid;  // 线程id

        static int _threadNum;  // 线程编号
    };
    int myThread::thread::_threadNum = 1;
}