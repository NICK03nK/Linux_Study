#pragma once

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include "log.hpp"
#include "err.h"

static const int defaultValue = -1;
static const int size = 128;

class Epoller
{
public:
    Epoller()
        : _epfd(defaultValue)
    {
    }

    ~Epoller()
    {
        if (_epfd != defaultValue)
        {
            close(_epfd);
        }
    }

public:
    void Create()
    {
        _epfd = epoll_create(size);
        if (_epfd < 0)
        {
            logMessage(FATAL, "epoll_create() error, code: %d, err string: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERROR);
        }
    }

    // user->kernel
    bool AddEvent(int sockFd, uint32_t events)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sockFd;

        int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockFd, &ev);

        return ret == 0;
    }

    // kernel->user
    int Wait(struct epoll_event* revs, int num, int timeout)
    {
        int ret = epoll_wait(_epfd, revs, num, timeout);
        return ret;
    }

private:
    int _epfd;
};