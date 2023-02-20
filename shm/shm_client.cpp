#include "comm.h"
#include <unistd.h>

int main()
{
    key_t k = getKey();
    printf("key: 0x%x\n", k);

    // 获取共享内存
    int shmid = getShm(k);
    printf("shmid: %d\n", shmid);

    // 将用户端与共享内存挂接
    char* start = (char*)attchShm(shmid);
    printf("attch success, address start: %p\n", start);

    // 用户端向服务端发送信息
    const char* msg = "hello server, 我是用户端，正在和你通信";
    pid_t id = getpid();
    int cnt = 1;
    while (true)
    {
        snprintf(start, MAX_SIZE, "%s[pid: %d][信息编号: %d]", msg, id, cnt++);  // 将共享内存的空间直接看成一块存放字符串的空间，直接将信息写入start中
        sleep(1);
    }
    
    // 将用户端与共享内存去关联
    detachShm(start);

    return 0;
}