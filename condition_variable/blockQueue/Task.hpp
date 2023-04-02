#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <cstdio>
using namespace std;

const string operation = "+-*/%";
int myMath(int x, int y, char op)
{
    int result = 0;
    switch (op)
    {
    case '+':
        result = x + y;
        break;

    case '-':
        result = x - y;
        break;

    case '*':
        result = x * y;
        break;

    case '/':
    {
        if (y == 0)
        {
            cerr << "div zero error" << endl;
            result = -1;
        }
        else
        {
            result = x / y;
        }
    }
        break;

    case '%':
    {
        if (y == 0)
        {
            cerr << "mod zero error" << endl;
            result = -1;
        }
        else
        {
            result = x % y;
        }
    }
        break;

    default:
        break;
    }

    return result;
}

// 计算任务类
class CalTask
{
    using func_t = function<int(int, int, char)>;
public:
    CalTask()
    {}

    CalTask(int x, int y, char op, func_t callBack)
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

void saveResult(const string& result)
{
    const string target = "./log.txt";
    FILE* fp = fopen(target.c_str(), "a+");
    if (!fp)
    {
        cerr << "fopen error" << endl;
        return;
    }

    fputs(result.c_str(), fp);
    fputs("\n", fp);

    fclose(fp);
}

// 保存任务类
class SaveTask
{
    using func_t = function<void(const string&)>;
public:
    SaveTask()
    {}

    SaveTask(const string& msg, func_t func)
    :_msg(msg)
    , _func(func)
    {}

    void operator()()
    {
        _func(_msg);
    }

private:
    string _msg;
    func_t _func;
};