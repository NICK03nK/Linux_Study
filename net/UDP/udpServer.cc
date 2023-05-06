#include "udpServer.hpp"
#include "onlineUsers.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>
#include <cstdio>

using namespace std;
using namespace Server;

void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " server_port\n\n";
}

// 业务1
// 实现一个简易的英翻中的业务逻辑
unordered_map<string, string> dict;

// 将txtLine按“:”为分隔符将左边的放入key中，右边的放入value中
static bool cutString(const string& target, string* s1, string* s2)
{
    const string sep = ":";  // 以“:”作为分隔符

    size_t pos = target.find(sep);
    if (pos == string::npos)  // 没找到
    {
        cout << "There is a problem with the current word table" << endl;

        return false;
    }
    else
    {
        *s1 = target.substr(0, pos);
        *s2 = target.substr(pos + sep.size());

        return true;
    }
}

// 初始化翻译字典
static const string dictTxt = "./dict.txt";
static void initDict()
{
    ifstream in(dictTxt, std::ios::binary);
    if (!in.is_open())  // 检测到文件打开失败
    {
        cerr << "open file" << dictTxt << "error" << endl;
        exit(OPEN_ERROR);
    }

    string txtLine;  // 从文件中读取一行的数据存放到txtLine中
    string key, value;  // 将txtLine按“:”为分隔符将左边的放入key中，右边的放入value中
    while (getline(in, txtLine))  // 从文件对象in中读取一行的内容，放到txtLine中
    {
        if (cutString(txtLine, &key, &value))
        {
            dict.insert(make_pair(key, value));  // 将中英文对应插入到字典中
        }
    }

    in.close();  // 实际上当析构ifstream对象时，对象会自动关闭打开的文件

    cout << "load dictionary success" << endl;
}

void reload(int signo)
{
    (void)signo;
    initDict();
}

void CtoE(int sockFd, string client_ip, uint16_t client_port, string word)
{
    string responseMessage;

    auto it = dict.find(word);
    if (it == dict.end())  // 字典中不存在该词组
    {
        responseMessage = "unknown word";
    }
    else
    {
        responseMessage = it->second;
    }

    // 将翻译结果返回给客户端
    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(client_port);
    client.sin_addr.s_addr = inet_addr(client_ip.c_str());

    sendto(sockFd, responseMessage.c_str(), responseMessage.size(), 0, (struct sockaddr*)&client, sizeof(client));
}
// 业务1end

// 业务2
// 远端命令行参数交互业务逻辑
void remoteCmd(int sockFd, string client_ip, uint16_t client_port, string cmd)
{
    if (cmd.find("mv") != string::npos || cmd.find("rm") != string::npos || cmd.find("rmdir") != string::npos)
    {
        cerr << client_ip << ":" << client_port << "正在执行一个非法操作" << cmd << endl;
        return;
    }

    string response;  // 用于接收命令行参数的执行结果
    FILE* fp = popen(cmd.c_str(), "r");

    if (fp == nullptr)  // popen()调用失败
    {
        response = cmd + "exec failed";
    }
    else
    {
        char line[num] = { 0 };
        while (fgets(line, sizeof(line) - 1, fp))
        {
            response += line;
        }
    }

    pclose(fp);

    // 将命令行参数执行结果返回给客户端
    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(client_port);
    client.sin_addr.s_addr = inet_addr(client_ip.c_str());

    sendto(sockFd, response.c_str(), response.size(), 0, (struct sockaddr*)&client, sizeof(client));
}
// 业务2end

// 业务3
// 群聊-服务器转发消息
onlineUsers online_users;

void remoteMessage(int sockFd, string client_ip, uint16_t client_port, string message)
{
    if (message == "online")
    {
        online_users.addUser(client_ip, client_port);
    }

    if (message == "offline")
    {
        online_users.delUser(client_ip, client_port);
    }

    if (online_users.isOnline(client_ip, client_port))  // 当前用户已上线，就把该用户的信息转发给其他在线用户
    {
        // 将当前用户发送的信息转发给其他在线用户
        online_users.broadcastMessage(sockFd, client_ip, client_port, message);
    }
    else  // 当前用户未上线
    {
        string response = "请先输入：online，进行上线";

        struct sockaddr_in client;
        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(client_port);
        client.sin_addr.s_addr = inet_addr(client_ip.c_str());

        sendto(sockFd, response.c_str(), response.size(), 0, (struct sockaddr*)&client, sizeof(client));
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    uint16_t server_port = atoi(argv[1]);

    // initDict();  // 执行翻译业务前要先初始化字典
    // signal(3, reload);  // 热加载字典
    // unique_ptr<udpServer> uqSVR(new udpServer(CtoE, server_port));

    // unique_ptr<udpServer> uqSVR(new udpServer(remoteCmd, server_port));

    unique_ptr<udpServer> uqSVR(new udpServer(remoteMessage, server_port));

    uqSVR->initServer();
    uqSVR->start();

    return 0;
}