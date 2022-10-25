#include "process.h"

void Proc()
{
  int cnt = 0;
  char bar[NUM] = { 0 };
  const char* lable = "|\\-/|\\-/";

  while(cnt <= 100)
  {
    printf("[%-100s][%-3d%%][%c]\r", bar, cnt, lable[cnt%8]);
    fflush(stdout);

    bar[cnt++] = STYLE;

    usleep(50000);
  }
  printf("\n");
}
