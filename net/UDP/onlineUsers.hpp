#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <arpa/inet.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

struct User
{
    string _ip;
    uint16_t _port;

    User(const string& ip, const uint16_t& port)
        :_ip(ip)
        , _port(port)
    {}

    ~User()
    {}
};

class onlineUsers
{
public:
    onlineUsers()
    {}

    ~onlineUsers()
    {}

    void addUser(const string& client_ip, const uint16_t& client_port)
    {
        string client_id = client_ip + "-" + to_string(client_port);
        _users.insert(make_pair(client_id, User(client_ip, client_port)));
    }

    void delUser(const string& client_ip, const uint16_t& client_port)
    {
        string client_id = client_ip + "-" + to_string(client_port);
        _users.erase(client_id);
    }

    bool isOnline(const string& client_ip, const uint16_t& client_port)
    {
        string client_id = client_ip + "-" + to_string(client_port);
        return _users.find(client_id) == _users.end() ? false : true;
    }

    void broadcastMessage(int sockFd, const string& client_ip, const uint16_t& client_port, const string& message)
    {
        for (const auto& user : _users)
        {
            struct sockaddr_in client;
            bzero(&client, sizeof(client));
            client.sin_family = AF_INET;
            client.sin_port = htons(user.second._port);
            client.sin_addr.s_addr = inet_addr(user.second._ip.c_str());

            string broadcast_message = client_ip + "-" + to_string(client_port) + "# " + message;

            sendto(sockFd, broadcast_message.c_str(), broadcast_message.size(), 0, (struct sockaddr*)&client, sizeof(client));
        }
    }

private:
    unordered_map<string, User> _users;
};