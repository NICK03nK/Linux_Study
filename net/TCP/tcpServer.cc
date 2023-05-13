#include "tcpServer.hpp"
#include <memory>
#include <cstdlib>

using namespace Server;

void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " server_port\n\n";
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
    uqSVR->start();

    return 0;
}