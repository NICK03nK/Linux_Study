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
    class thread;  // 对thread类进行声明，context类才能找到thread

    class context
    {
    public:
        thread* _this;  // 用于保存线程对象
        void* _args;  // 用于保存线程所要执行的函数的参数

    public:
        context()
            :_this(nullptr)
            , _args(nullptr)
        {}

        ~context()
        {}
    };

    class thread
    {
        typedef function<void*(void*)> func_t;
        const int num = 1024;

        static void* start_routine(void* args)
        {
            // 静态成员函数无法调用非静态成员变量
            // return _func(_args);

            // 此处创建一个Context类来解决
            // Context类用于保存thread对象和thread对象要执行的函数的参数
            context* ctx = static_cast<context*>(args);
            void* ret = ctx->_this->run(ctx->_args);

            delete ctx;  // ctx对象是在start中new出来的，在此使用完毕后要delete释放

            return ret;
        }

    public:
        thread(func_t func, void* args, int number)
            :_func(func)
            , _args(args)
        {
            // 初始化线程名称
            char namebuffer[num];
            snprintf(namebuffer, sizeof(namebuffer), "thread-%d", number);
            _name = namebuffer;

            // 创建线程
            // 创建一个ctx对象，指向当前thread对象，以及将thread对象要执行的函数保存到ctx的_args中
            context* ctx = new context;
            ctx->_this = this;
            ctx->_args = _args;

            int n = pthread_create(&_tid, nullptr, start_routine, ctx);
            assert(n == 0);
            (void)n;
        }

        void join()
        {
            int n = pthread_join(_tid, nullptr);
            assert(n == 0);
            (void)n;
        }

        void* run(void* args)
        {
            return _func(args);  // 执行线程要调用的函数
        }

        ~thread()
        {}

    private:
        string _name;  // 线程名称
        func_t _func;  // 线程执行函数
        void* _args;  // 线程执行函数的参数
        pthread_t _tid;  // 线程id
    };
}