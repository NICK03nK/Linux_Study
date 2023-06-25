#pragma once

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class util
{
public:
    static string getOneLine(string& buffer, const string& sep)
    {
        size_t pos = buffer.find(sep);
        if (pos == string::npos)  // 没找到分隔符
        {
            return "";
        }

        string sub = buffer.substr(0, pos);
        buffer.erase(0, sub.size() + sep.size());  // 删除已提取的一行

        return sub;
    }

    static bool readFile(const string& resource, string* out)
    {
        ifstream in(resource);
        if (!in.is_open())  // 文件打开失败
        {
            return false;
        }

        string line;
        while (getline(in, line))  // 将resource中的数据按行读取
        {
            *out += line;  // 将resourcez中的数据全部写入到out中
        }

        in.close();  // 关闭文件
        return true;
    }
};