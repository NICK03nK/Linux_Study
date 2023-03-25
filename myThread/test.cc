#include "mythread.hpp"
#include <memory>
#include <string>
#include <unistd.h>

void* thread_run(void* args)
{
    string buffer = static_cast<const char*>(args);
    while (true)
    {
        cout << "新线程 >> " << buffer << endl;
        sleep(1);
    }
}

int main()
{
    unique_ptr<myThread::thread> trd1(new myThread::thread(thread_run, (void*)"test thread", 1));
    unique_ptr<myThread::thread> trd2(new myThread::thread(thread_run, (void*)"test thread", 2));
    unique_ptr<myThread::thread> trd3(new myThread::thread(thread_run, (void*)"test thread", 3));

    trd1->join();
    trd2->join();
    trd3->join();


    return 0;
}