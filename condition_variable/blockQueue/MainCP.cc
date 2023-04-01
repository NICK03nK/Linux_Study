#include "BlockQueue.hpp"
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include "Task.hpp"

const string operation = "+-*/%";
int myMath(int x, int y, char op)
{
    int result = 0;
    switch (op)
    {
    case '+':
        result = x + y;
        break;

    case '-':
        result = x - y;
        break;

    case '*':
        result = x * y;
        break;

    case '/':
    {
        if (y == 0)
        {
            cerr << "div zero error" << endl;
            result = -1;
        }
        else
        {
            result = x / y;
        }
    }
        break;

    case '%':
    {
        if (y == 0)
        {
            cerr << "mod zero error" << endl;
            result = -1;
        }
        else
        {
            result = x % y;
        }
    }
        break;

    default:
        break;
    }

    return result;
}

void* consume(void* arg)
{
    BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(arg);

    while (true)
    {
        // 消费活动
        Task t;
        bq->pop(&t);

        cout << "消费任务: " << t() << endl;
        sleep(1);
    }

    return nullptr;
}

void* produce(void* arg)
{
    BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(arg);

    while (true)
    {
        // 生产活动
        int x = rand() % 100 + 1;
        int y = rand() % 10;
        int operCode = rand() % operation.size();

        Task t(x, y, operation[operCode], myMath);
        bq->push(t);

        cout << "生产任务: " << t.toTaskString() << endl;
    }

    return nullptr;
}

int main()
{
    srand((unsigned long)time(nullptr) ^ getpid());

    // 创建一个阻塞队列对象，让消费者线程和生产者线程都能看到这个对象
    BlockQueue<Task>* bq = new BlockQueue<Task>();

    // 创建一个消费者线程，一个生产者线程
    pthread_t c, p;
    pthread_create(&c, nullptr, consume, bq);
    pthread_create(&p, nullptr, produce, bq);

    // 回收消费者线程和生产者线程
    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    // 释放阻塞队列
    delete bq;
    
    return 0;
}