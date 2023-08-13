#pragma once

#include <iostream>

enum // 错误码
{
    USE_ERROR = 1,
    SOCK_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    EPOLL_CREATE_ERROR
};