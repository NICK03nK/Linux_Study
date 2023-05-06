#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <strings.h>
#include <netinet/in.h>
#include <pthread.h>

using namespace std;

namespace Client
{
    static const int num = 1024;

    enum {USE_ERROR = 1, SOCK_ERROR};

    class udpClient
    {
    public:
        udpClient(const string& ip, const uint16_t port)
            :_ip(ip)
            , _port(port)
            , _sockFd(-1)
            , _quit(false)
        {}

        void initClient()
        {
            // 调用socket()创建socket
            _sockFd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockFd == -1)
            {
                cerr << "socket error" << errno << " : " << strerror(errno) << endl;
                exit(SOCK_ERROR);
            }

            // client不需要显示bind()，OS会自动调用bind()
        }

        static void* recvMessage(void* arg)
        {
            pthread_detach(pthread_self());

            int sockFd = *(static_cast<int*>(arg));

            while (true)
            {
                // 接收服务器的发回的响应
                char buffer[num] = { 0 };
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                ssize_t ret = recvfrom(sockFd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&peer, &len);

                if (ret > 0)
                {
                    // 业务1的响应逻辑
                    // buffer[ret] = 0;
                    // if (strcmp(buffer, "unknown word") == 0)
                    // {
                    //     cout << "error# " << buffer << endl;
                    // }
                    // else
                    // {
                    //     cout << "translate# " << buffer << endl;
                    // }

                    // 业务2的响应逻辑
                    // buffer[ret] = 0;
                    // cout << "response# " << endl;
                    // cout << buffer << endl;

                    // 业务3的响应逻辑
                    buffer[ret] = 0;
                    cout << buffer << endl;
                }
            }

            return nullptr;
        }

        void run()
        {
            // 利用多线程将client端的sendto和recvfrom分离开来(sendto和recvfrom都采用的是阻塞式，若不分离则达不到业务3的要求：不发消息也能看到群聊消息)
            pthread_create(&_reader, nullptr, recvMessage, (void*)&_sockFd);  // 创建一个从线程来专门负责读取服务器发回的消息

            struct sockaddr_in server;
            bzero(&server, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(_port);
            server.sin_addr.s_addr = inet_addr(_ip.c_str());

            string request;
            while (!_quit)
            {
                // cout << "Enter# ";
                fprintf(stderr, "Enter# ");
                fflush(stderr);
                getline(cin, request);
                cout << endl;

                // 向服务器发送请求
                sendto(_sockFd, request.c_str(), request.size(), 0, (struct sockaddr*)&server, sizeof(server));
            }
        }

        ~udpClient()
        {}

    private:
        string _ip;
        uint16_t _port;
        int _sockFd;

        bool _quit;

        pthread_t _reader;
    };
}