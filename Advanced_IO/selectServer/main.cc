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

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    unique_ptr<selectServer> uqSVR(new selectServer(atoi(argv[1])));

    uqSVR->initServer();
    uqSVR->start();

    return 0;
}