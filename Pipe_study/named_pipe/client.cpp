#include "comm.hpp"

int main()
{
    int writeFd = open(NAMED_PIPE, O_WRONLY);
    if (writeFd < 0)
    {
        exit(1);
    }

    char buffer[1024] = { 0 };
    while (true)
    {
        cout << "Please say >> ";
        
        fgets(buffer, sizeof(buffer), stdin);
        if (strlen(buffer) > 0)
        {
            buffer[strlen(buffer) - 1] = '\0';
        }

        ssize_t s = write(writeFd, buffer, strlen(buffer));
        assert(s == strlen(buffer));
        (void)s;
    }
    close(writeFd);
    
    return 0;
}