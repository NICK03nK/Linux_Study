#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define PATHNAME "."
#define PROJ_ID 0x66
#define MAX_SIZE 4096

// 将获取key值这一步骤封装起来，以确保pathname和proj_id都是一致的
key_t getKey()
{
    key_t k = ftok(PATHNAME, PROJ_ID);

    if (k < 0)
    {
        std::cerr << "ftok" << errno << ":" << strerror(errno) << std::endl;
        exit(1);
    }

    return k;
}

int getShmHelper(key_t key, int flags)
{
    int shmid = shmget(key, MAX_SIZE, flags);

    if (shmid < 0)
    {
        std::cerr << "shmget" << errno << ":" << strerror(errno) << std::endl;
        exit(2);
    }

    return shmid;
}

// 创建共享内存
int createShm(key_t key)
{
    return getShmHelper(key, IPC_CREAT | IPC_EXCL | 0600);  // 0600给创建的共享内存设置权限，0600 -> 拥有者有读写权限
}

// 获取共享内存
int getShm(key_t key)
{
    return getShmHelper(key, IPC_CREAT);
}

// 将进程与共享内存空间挂接
void* attchShm(int shmid)
{
    void* mem = shmat(shmid, nullptr, 0);

    //if ((int)mem == -1)  // 由于Linux环境下是64位的操作系统，指针大小为8字节，所以强转为int会报精度缺失的错误，要强转为long long
    if ((long long)mem == -1L)
    {
        std::cerr << "shmat" << errno << ":" << strerror(errno) << std::endl;
        exit(4);
    }

    return mem;
}

// 将进程与共享内存去关联
void detachShm(const void* start)
{
    if (shmdt(start) == -1)
    {
        std::cerr << "shmdt" << errno << ":" << strerror(errno) << std::endl;
        exit(5);
    }
}

// 删除共享内存
void delShm(int shmid)
{
    if (shmctl(shmid, IPC_RMID, nullptr) == -1)
    {
        std::cerr << "shmctl" << errno << ":" << strerror(errno) << std::endl;
        exit(3);
    }
}