#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>

class Util
{
public:
    static bool setNoneBlock(int fd)
    {
        int fl = fcntl(fd, F_GETFL);
        if (fl < 0)
        {
            return false;
        }

        fcntl(fd, F_SETFL, fl | O_NONBLOCK);

        return true;
    }
};