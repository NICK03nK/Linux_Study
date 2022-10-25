#include <stdio.h>
#include <unistd.h>

int main()
{
  int i = 0;
  for (i = 10; i >= 0; --i)
  {
    printf("剩余时间:%2d\r", i);

    fflush(stdout);

    sleep(1);
  }

  return 0;
}
