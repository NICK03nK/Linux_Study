#include "epollServer.hpp"
#include <memory>

using namespace std;
using namespace Server;

void Usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " port\n\n";
}

string echo(const string& message)
{
    return "I am epollServer, " + message;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    uint16_t port = atoi(argv[1]);

    unique_ptr<epollServer> uqSVR(new epollServer(echo, port));

    uqSVR->initServer();
    uqSVR->start();

    return 0;
}