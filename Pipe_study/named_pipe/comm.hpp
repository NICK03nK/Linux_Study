#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
using namespace std;

#define NAMED_PIPE "./named_pipe"

bool creatFifo(const string& path)
{
    umask(0);
    int n = mkfifo(path.c_str(), 0600);
    if (n == 0)
    {
        return true;
    }
    else
    {
        cout << "mkfifo error : " << errno << " error string >> " << strerror(errno) << endl;
        return false;
    }
}

void removeFifo(const string& path)
{
    int n = unlink(path.c_str());
    assert(n == 0);
    (void)n;
}