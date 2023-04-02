#include "BlockQueue.hpp"
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include "Task.hpp"

// 消费
void* consume(void* arg)
{
    // 获取main函数中的bqs中的计算任务的阻塞队列对象
    BlockQueue<CalTask>* c_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(arg))->c_bq;
    // 获取main函数中的bqs中的保存任务的阻塞队列对象
    BlockQueue<SaveTask>* s_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(arg))->s_bq;

    while (true)
    {
        // 消费活动
        // 1. 处理计算任务的阻塞队列中的计算任务
        CalTask ct;
        c_bq->pop(&ct);
        string result = ct();
        cout << "c_thread >> 完成计算任务: " << result << endl;
        
        // 生产活动
        // 2. 向保存任务的阻塞队列中发送保存任务
        SaveTask st(result, saveResult);
        s_bq->push(st);

        cout << "c_thread >> 推送保存任务..." << endl;
    }

    return nullptr;
}

// 生产
void* produce(void* arg)
{
    // 获取main函数中的bqs中的计算任务的阻塞队列对象
    BlockQueue<CalTask>* c_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(arg))->c_bq;

    while (true)
    {
        // 生产活动
        // 向计算任务的阻塞队列中发送计算任务
        int x = rand() % 100 + 1;
        int y = rand() % 10;
        int operCode = rand() % operation.size();

        CalTask ct(x, y, operation[operCode], myMath);
        c_bq->push(ct);

        cout << "p_thread >> 生产计算任务: " << ct.toTaskString() << endl;

        sleep(1);
    }

    return nullptr;
}

// 保存
void* save(void* arg)
{
    // 获取main函数中的bqs中的保存任务的阻塞队列对象
    BlockQueue<SaveTask>* s_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(arg))->s_bq;

    while (true)
    {
        // 消费活动
        // 处理保存任务的阻塞队列中的保存任务
        SaveTask st;
        s_bq->pop(&st);
        st();
        cout << "s_thread >> 完成保存任务" << endl;
    }
}

int main()
{
    srand((unsigned long)time(nullptr) ^ getpid());

    BlockQueues<CalTask, SaveTask> bqs;

    // 创建一个计算任务的阻塞队列对象，让消费者线程和生产者线程都能看到这个对象
    bqs.c_bq = new BlockQueue<CalTask>();
    // 创建一个保存任务的阻塞队列对象，让消费者线程和生产者线程都能看到这个对象
    bqs.s_bq = new BlockQueue<SaveTask>();

    // p是纯生产者线程，s是纯消费者线程
    // c既是消费者线程，也是生产者线程
    pthread_t c, p, s;
    pthread_create(&c, nullptr, consume, &bqs);
    pthread_create(&p, nullptr, produce, &bqs);
    pthread_create(&s, nullptr, save, &bqs);

    // 回收三个线程
    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    pthread_join(s, nullptr);

    // 释放两个阻塞队列
    delete bqs.c_bq;
    delete bqs.s_bq;
    
    return 0;
}