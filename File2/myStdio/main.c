#include "myStdio.h"
#include <stdio.h>

int main()
{
  FILE_* fp = fopen_("./log.txt", "w");
  if (fp == NULL)
  {
    return 1;
  }

  const char* msg = "hello_file  ";

  int cnt = 10;
  while (cnt--)
  {
    fwrite_(msg, strlen(msg), fp);
    fflush_(fp);
    sleep(1);
    printf("count: %d\n", cnt);
  }

  fclose_(fp);  // 当调用fcloes_时，函数内部会调用fflush_，fflush_又会调用系统接口write，将缓冲区中的数据刷新写入文件中

  return 0;
}
