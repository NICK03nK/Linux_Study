#include "httpServer.hpp"
#include <memory>

using namespace std;
using namespace Server;

void Usage(string proc)
{
    cerr << "Usage:\n\t" << proc << " port\r\n\r\n";
}

bool Get(const httpRequest& req, const httpResponse& resp)
{
    // for test
    cout << "-------------------http start-------------------" << endl;
    cout << req.inbuffer << endl;
    cout << "--------------------http end--------------------" << endl;

    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port = atoi(argv[1]);

    unique_ptr<httpServer> httpSvr(new httpServer(port, Get));
    httpSvr->initServer();
    httpSvr->start();

    return 0;
}