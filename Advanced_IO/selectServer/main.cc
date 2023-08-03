#include "selectServer.hpp"
#include "err.h"
#include <memory>
#include <stdlib.h>

using namespace std;
using namespace Server;

void Usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " port\n\n";
}

std::string transaction(const std::string& request)
{
    return request;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    unique_ptr<selectServer> uqSVR(new selectServer(transaction, atoi(argv[1])));

    uqSVR->initServer();
    uqSVR->start();

    return 0;
}