#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <cstdio>
using namespace std;

class Task
{
    using func_t = function<int(int, int, char)>;
public:
    Task()
    {}

    Task(int x, int y, char op, func_t callBack)
        :_x(x)
        , _y(y)
        , _op(op)
        , _callBack(callBack)
    {}

    string operator()()
    {
        int result = _callBack(_x, _y, _op);
        char buffer[1024] = { 0 };
        snprintf(buffer, sizeof(buffer), "%d %c %d = %d", _x, _op, _y, result);

        return buffer;
    }

    string toTaskString()
    {
        char buffer[1024] = { 0 };
        snprintf(buffer, sizeof(buffer), "%d %c %d = ?", _x, _op, _y);

        return buffer;
    }

private:
    int _x;
    int _y;
    char _op;
    func_t _callBack;
};