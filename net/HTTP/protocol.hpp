#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class httpRequest
{
public:
    string inbuffer;

    string requestLine;  // 请求行
    vector<string> requestHeader;  // 请求报头
    string body;  // 正文

    string method;  // 请求行的方法
    string url;  // 请求行的url
    string httpVersion; // http协议版本
};

class httpResponse
{
public:
    string outbuffer;
};