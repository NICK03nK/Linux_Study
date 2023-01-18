#include "comm.hpp"

int main()
{
    bool n = creatFifo(NAMED_PIPE);
    assert(n);
    (void)n;

    int readFd = open(NAMED_PIPE, O_RDONLY);
    if (readFd < 0)
    {
        exit(1);
    }

    char buffer[1024] = { 0 };
    while (true)
    {
        ssize_t s = read(readFd, buffer, sizeof(buffer) - 1);

        if (s > 0)
        {
            buffer[s] = '\0';
            cout << "client -> server # " << buffer << endl;
        }
        else if (s == 0)
        {
            cout << "client quit, so do i" << endl;
            break;
        }
    }
    close(readFd);

    removeFifo(NAMED_PIPE);

    return 0;
}