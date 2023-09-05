#include <memory>

#include "tcpServer.hpp"

using namespace Server;

void Usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " port\n\n";
}

bool Cal(const Request& req, Response& resp)
{
    resp.clear();  // 清空Response对象中的数据，_exitCode默认设为0

    switch(req._op)
    {
    case '+':
        resp._result = req._x + req._y;
        break;
    case '-':
        resp._result = req._x - req._y;
        break;
    case '*':
        resp._result = req._x * req._y;
        break;
    case '/':
        if (req._y == 0)
        {
            resp._exitCode = DIV_ZERO;
        }
        else
        {
            resp._result = req._x / req._y;
        }
        break;
    case '%':
        if (req._y == 0)
        {
            resp._exitCode = MOD_ZERO;
        }
        else
        {
            resp._result = req._x % req._y;
        }
        break;
    default:
        resp._exitCode = OP_ERROR;
        break;
    }

    return true;
}

void calculate(Connection *conn)
{
    std::string onePackage;
    while (ParseOnePackage(conn->_inbuffer, &onePackage))
    {
        std::string reqStr;
        if (!deLength(onePackage, &reqStr))
        {
            return;
        }

        std::cout << "去掉报头的正文: \n" << reqStr << std::endl;

        Request req;
        if (!req.deserialize(reqStr))
        {
            return; // 反序列化失败，直接返回
        }

        Response resp;
        Cal(req, resp);

        // 将Response对象做序列化处理
        std::string respStr;
        resp.serialize(&respStr);

        // 给响应添加报头封装成报文
        conn->_outbuffer += enLength(respStr);
        std::cout << "result:\n" << conn->_outbuffer << std::endl;
    }

    // 将处理完的数据发回给对端
    if (conn->_sender)
    {
        conn->_sender(conn);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    uint16_t port = atoi(argv[1]);

    std::unique_ptr<tcpServer> uqSVR(new tcpServer(calculate, port));

    uqSVR->InitServer();
    uqSVR->Dispatcher();

    return 0;
}

