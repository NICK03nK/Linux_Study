#include "udpServer.hpp"
#include <memory>

using namespace std;
using namespace Server;

void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " server_port\n\n";
}

void handlerMessage(string client_ip, uint16_t client_port, string message)
{
    cout << client_ip << "[" << client_port << "]# " << message << endl;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    uint16_t server_port = atoi(argv[1]);

    unique_ptr<udpServer> uqSVR(new udpServer(handlerMessage, server_port));

    uqSVR->initServer();
    uqSVR->start();

    return 0;
}