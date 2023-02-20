#include "comm.h"
#include <unistd.h>

int main()
{
    key_t k = getKey();
    printf("key: 0x%x\n", k);

    // 创建共享内存
    int shmid = createShm(k);
    printf("shmid: %d\n", shmid);
    sleep(5);

    // 将服务端与共享内存挂接
    char* start = (char*)attchShm(shmid);
    printf("attch success, address start: %p\n", start);

    // 服务端接收用户端发送的信息
    while (true)
    {
        printf("client say: %s\n", start);  // 将共享内存的空间直接看成一块存放字符串的空间，直接从start中读取信息

        // 获取共享内存的属性信息
        struct shmid_ds ds;
        shmctl(shmid, IPC_STAT, &ds);
        printf("获取shm信息>> size: %d, creatorPid: %d, myPid: %d, key: 0x%x\n", ds.shm_segsz, ds.shm_cpid, getpid(), ds.shm_perm.__key);

        sleep(1);
    }

    // 服务端与共享内存去关联
    detachShm(start);
    printf("detach success\n");
    sleep(5);


    // 删除共享内存
    delShm(shmid);  // 谁创建谁删除

    return 0;
}