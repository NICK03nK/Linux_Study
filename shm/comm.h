#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>

#define PATHNAME "."
#define PROJ_ID 0x66
#define MAX_SIZE 4096

// 将获取key值这一步骤封装起来，以确保pathname和proj_id都是一致的
key_t getKey()
{
    key_t k = ftok(PATHNAME, PROJ_ID);
    if (k < 0)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(1);
    }

    return k;
}


int getShmHelper(key_t key, int flags)
{
    int shmid = shmget(key, MAX_SIZE, flags);
    if (shmid < 0)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(2);
    }

    return shmid;
}

int createShm(key_t key)
{
    return getShmHelper(key, IPC_CREAT | IPC_EXCL);
}

int getShm(key_t key)
{
    return getShmHelper(key, IPC_CREAT);
}