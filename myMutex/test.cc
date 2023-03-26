#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include "myMutex.hpp"
using namespace std;

int ticket = 100;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *route(void *arg)
{
    char *id = (char *)arg;
    while (1)
    {
        {  // 这一个代码块是临界区，出了这个作用域，自动解锁
            LockGuard lockGuard(&mtx);  // RAII风格的自动加锁解锁
            if (ticket > 0)
            {
                usleep(1000);
                printf("%s sells ticket:%d\n", id, ticket);
                ticket--;
            }
            else
            {
                break;
            }
        }

        usleep(1000);
    }
}

int main()
{
    pthread_t t1, t2, t3, t4;

    pthread_create(&t1, NULL, route, (void*)"thread 1");
    pthread_create(&t2, NULL, route, (void*)"thread 2");
    pthread_create(&t3, NULL, route, (void*)"thread 3");
    pthread_create(&t4, NULL, route, (void*)"thread 4");

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    return 0;
}
