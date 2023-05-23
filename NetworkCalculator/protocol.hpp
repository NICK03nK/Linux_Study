#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

#define SEP " "
#define SEP_LEN strlen(SEP)
#define LINE_SEP "\r\n"
#define LINE_SEP_LEN strlen(LINE_SEP)

enum {DIV_ZERO = 1, MOD_ZERO, OP_ERROR};

// 封装：给有效载荷添加报头
// "text" -> "text_len"\r\n"text"\r\n
string enLength(const string& text)
{
    string package = to_string(text.size());
    package += LINE_SEP;
    package += text;
    package += LINE_SEP;

    return package;
}

// 解包：将报头和有效载荷分离
// "text_len"\r\n"text"\r\n -> "text"
string deLength(const string& package)
{
    size_t pos = package.find(LINE_SEP);
    string text_len_str = package.substr(0, pos);
    int text_len = stoi(text_len_str);
    string text = package.substr(pos + LINE_SEP_LEN, text_len);

    return text;
}

class Request
{
public:
    Request()
        :_x(0)
        , _y(0)
        , _op(0)
    {}

    Request(int x, int y, char op)
        :_x(x)
        , _y(y)
        , _op(op)
    {}

    bool serialize(string* out)
    {
        *out = "";  // 将out清空
        
        // 将结构化数据序列化为 -> "x op y"
        string x_str = to_string(_x);
        string y_str = to_string(_y);

        *out = x_str;
        *out += SEP;
        *out += _op;
        *out += SEP;
        *out += y_str;

        return true;
    }

    bool deserialize(const string& in)
    {
        // 将"x op y"反序列化为 -> 结构化数据
        size_t leftSEP = in.find(SEP);
        size_t rightSEP = in.rfind(SEP);

        if (leftSEP == string::npos || rightSEP == string::npos)  // 报文中缺少分隔符
        {
            return false;
        }

        if (leftSEP == rightSEP)  // 报文中缺少操作符
        {
            return false;
        }

        if (rightSEP - (leftSEP + SEP_LEN) != 1)  // 操作符格式错误
        {
            return false;
        }

        string x_str = in.substr(0, leftSEP);
        string y_str = in.substr(rightSEP + SEP_LEN);
        if (x_str.empty() || y_str.empty())
        {
            return false;
        }

        _x = stoi(x_str);
        _y = stoi(y_str);
        _op = in[leftSEP + SEP_LEN];

        return true;
    }

public:
    int _x;  // 第一个操作数
    int _y;  // 第二个操作数
    char _op;  // 操作符 -- (+ - * / %)
};

class Response
{
public:
    Response()
        :_exitCode(0)
        , _result(0)
    {}

    bool serialize(string* out)
    {
        // 将结构化数据序列化为 -> "exitCode result"
        string ec_str = to_string(_exitCode);
        string res_str = to_string(_result);

        *out = ec_str;
        *out += SEP;
        *out += res_str;

        return true;
    }

    bool deserialize(const string& in)
    {
        // 将"exitCode result"反序列化为 -> 结构化数据
        size_t mid = in.find(SEP);

        if (mid == string::npos)
        {
            return false;
        }

        string ec_str = in.substr(0, mid);
        string res_str = in.substr(mid + SEP_LEN);
        if (ec_str.empty() || res_str.empty())
        {
            return false;
        }

        _exitCode = stoi(ec_str);
        _result = stoi(res_str);

        return true;
    }

    void clear()
    {
        _exitCode = 0;
        _result = 0;
    }

public:
    int _exitCode;  // 0：计算结果正确，!0：计算结果错误，作废
    int _result;  // 计算结果
};

// 在接收端接收缓冲区中："text_len"\r\n"text"\r\n"text_len"\r\n"text"\r\n"text_len"\r\n"text"\r\n
bool GetPackage(int sockFd, string& inbuffer, string* package)
{
    char buffer[1024] = { 0 };
    while (true)  // 循环式接收，直到读取到一个完整的报文才跳出循环
    {
        ssize_t n = recv(sockFd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = 0;
            inbuffer += buffer;

            // 1.找到第一个LINE_SEP，从而拿到"text_len"
            auto pos = inbuffer.find(LINE_SEP);
            if (pos == string::npos)  // 说明inbuffer中还没有LINE_SEP
            {
                continue;  // 继续执行以上代码，继续读取
            }

            // 代码执行到这，说明已经拿到了"text_len"
            string text_len_str = inbuffer.substr(0, pos);
            int text_len = stoi(text_len_str);

            // 计算一个完整报文的长度
            int package_len = text_len_str.size() + 2 * LINE_SEP_LEN + text_len;

            if (inbuffer.size() < package_len)  // inbuffer中还没读取到一个完整的报文
            {
                continue;
            }

            // 代码执行到这，说明inbuffer中已经有一个完整报文了
            *package = inbuffer.substr(0, package_len);
            inbuffer.erase(0, package_len);  // 从缓冲区中拿走了一个完整的报文

            break;
        }
        else
        {
            return false;
        }
    }

    return true;
}