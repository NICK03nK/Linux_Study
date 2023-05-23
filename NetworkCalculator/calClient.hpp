#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cctype>

#include "protocol.hpp"

using namespace std;

namespace Client
{
    enum {USE_ERROR = 1, SOCK_ERROR};

    class tcpClient
    {
    public:
        tcpClient(const string& link_ip, const uint16_t& link_port)
            :_serverIp(link_ip)
            , _serverPort(link_port)
            , _sockFd(-1)
        {}

        void initClient()
        {
            // 创建套接字
            _sockFd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockFd == -1)
            {
                cerr << "create socket error" << endl;
                exit(SOCK_ERROR);
            }
        }

        void run()
        {
            // 与服务器建立链接
            struct sockaddr_in server;
            bzero(&server, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(_serverPort);
            server.sin_addr.s_addr = inet_addr(_serverIp.c_str());

            int ret = connect(_sockFd, (struct sockaddr*)&server, sizeof(server));
            if (ret == 0)  // 建立链接成功
            {
                string expression;
                while (true)
                {
                    cout << "Enter# ";
                    getline(cin, expression);

                    // 测试
                    Request req = Parse(expression);
                    string text;
                    req.serialize(&text);  // 拿到序列化数据

                    // 将序列化数据添加报头封装成报文，发送给服务器
                    string sendPackage = enLength(text);
                    send(_sockFd, sendPackage.c_str(), sendPackage.size(), 0);

                    // 接收服务器发回的响应(报文)
                    string inbuffer;
                    string recvPackage;

                    if (!GetPackage(_sockFd, inbuffer, &recvPackage))
                    {
                        continue;
                    }

                    string respMessage = deLength(recvPackage);  // 将服务器发回的响应报文解包，拿到有效载荷
                    Response resp;
                    resp.deserialize(respMessage);  // 将该有效载荷反序列化，得到一个Response对象(结构化数据)

                    cout << "exitCode: " << resp._exitCode << endl;
                    cout << "result: " << resp._result << endl << endl;
                }
            }
            else  // 建立链接失败
            {
                cerr << "socket connect error" << endl;
            }
        }

        ~tcpClient()
        {
            
        }

    private:
        Request Parse(const string& expression)
        {
            int status = 0;  // 0：操作符左侧  1：操作符  2：操作符右侧
            int i = 0;
            int cnt = expression.size();
            string left, right;
            char op = 0;

            while (i < cnt)
            {
                switch (status)
                {
                case 0:
                    if (isdigit(expression[i]))
                    {
                        left.push_back(expression[i++]);
                    }
                    else
                    {
                        op = expression[i];
                        status = 1;
                    }
                    break;
                case 1:
                    ++i;
                    status = 2;
                    break;
                case 2:
                    right.push_back(expression[i++]);
                    break;
                }
            }

            return Request(stoi(left), stoi(right), op);
        }

    private:
        string _serverIp;  // 要连接服务器的主机ip
        uint16_t _serverPort;  // 要连接服务的器程序的port
        int _sockFd;
    };
}