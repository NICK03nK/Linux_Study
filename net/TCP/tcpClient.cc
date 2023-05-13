#include "tcpClient.hpp"
#include <memory>
#include <cstdlib>

using namespace Client;

void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " link_ip link_port\n\n";
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(USE_ERROR);
    }

    string link_ip = argv[1];
    uint16_t link_port = atoi(argv[2]);

    unique_ptr<tcpClient> uqClt(new tcpClient(link_ip, link_port));

    uqClt->initClient();
    uqClt->run();

    return 0;
}