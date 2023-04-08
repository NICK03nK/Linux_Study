#include "ThreadPool.hpp"
#include "Task.hpp"
#include <memory>

int main()
{
    unique_ptr<ThreadPool<Task>> uq(new ThreadPool<Task>());
    uq->run();

    int x = 0, y = 0;
    char op = 0;
    while (true)
    {
        cout << "请输入数据1# ";
        cin >> x;
        cout << "请输入数据2# ";
        cin >> y;
        cout << "请输入运算符# ";
        cin >> op;
        Task t(x, y, op, myMath);

        cout << "你录入了一个任务" << t.toTaskString() << " 确认提交吗？[y/n]# ";
        char confirm = 0;
        cin >> confirm;
        if (confirm == 'y')
        {
            uq->push(t);
        }

        sleep(1);
    }

    return 0;
}


// ------------------------------ ONLY FOR TEST ------------------------------

// int main()
// {   
//     unique_ptr<ThreadPool<int>> uq(new ThreadPool<int>());
//     uq->run();

//     while (true)
//     {
//         sleep(1);
//     }
    
//     return 0;
// }

// #include "ThreadPool.hpp"
// #include "myThread.hpp"
// #include <unistd.h>

// void* run(void* arg)
// {
//     myThread::thread* pt = static_cast<myThread::thread*>(arg);

//     while (true)
//     {
//         cout << pt->getThreadName() << endl;
//         sleep(1);
//     }

//     return nullptr;
// }

// int main()
// {
//     myThread::thread t1(run, &t1);
//     myThread::thread t2(run, &t2);
//     myThread::thread t3(run, &t3);

//     t1.start();
//     t2.start();
//     t3.start();

//     t1.join();
//     t2.join();
//     t3.join();

//     return 0;
// }