#include "calServer.hpp"
#include <memory>
#include <cstdlib>

using namespace Server;

void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " server_port\n\n";
}

bool Calculate(const Request& req, Response& resp)
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

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    uint16_t server_port = atoi(argv[1]);

    unique_ptr<tcpServer> uqSVR(new tcpServer(server_port));
    
    uqSVR->initServer();
    uqSVR->start(Calculate);

    return 0;
}