#include "comm.h"

int main()
{
    key_t k = getKey();
    printf("key: 0x%x\n", k);

    int shmid = createShm(k);
    printf("shmid: %d\n", shmid);

    return 0;
}