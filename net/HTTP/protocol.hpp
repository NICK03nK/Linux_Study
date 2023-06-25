#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.hpp"

using namespace std;

const string sep = "\r\n";  // 定义的行分隔符
const string default_root = "./wwwroot";
const string home_page = "index.html";  // 默认首页
const string html_404 = "./wwwroot/404.html";

class httpRequest
{
public:
    httpRequest()
    {}

    ~httpRequest()
    {}

    void parse()
    {
        // 1.从req中拿到完整的请求行
        string line = util::getOneLine(inbuffer, sep);
        if (line.empty())
        {
            return;
        }

        // 2.从请求行中提取三个字段(method，url，version)
        stringstream ss(line);
        ss >> method >> url >> httpVersion;  // stringstream对象自动以空格为分隔符，填入到定义好的变量中

        // 3.添加web默认路径
        path += default_root;  // ./wwwroot
        path += url;

        if (path[path.size() - 1] == '/')  // 没有指定访问的资源，默认访问web主页
        {
            path += home_page;
        }

        // 4.获取资源文件的后缀
        size_t pos = path.rfind(".");
        if (pos == string::npos)
        {
            suffix = ".html";
        }
        else
        {
            suffix = path.substr(pos);  // 获取后缀
        }

        // 5.获取要访问的资源的大小
        struct stat st;
        int n = stat(path.c_str(), &st);
        if (n == 0)
        {
            size = st.st_size;
        }
        else
        {
            size = -1;
        }
    }

public:
    string inbuffer;

    // string requestLine;  // 请求行
    // vector<string> requestHeader;  // 请求报头
    // string body;  // 正文

    string method;  // 请求行的方法
    string url;  // 请求行的url
    string httpVersion; // http协议版本

    string path;  // 资源路径
    string suffix;  // 资源文件的后缀
    int size;  // 响应正文的大小
};

class httpResponse
{
public:
    string outbuffer;
};