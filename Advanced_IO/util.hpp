#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>

void setNoneBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        std::cerr << "fcntl: " << strerror(errno) << std::endl;
        return;
    }

    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}