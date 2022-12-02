#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
  // C接口
  printf("hello printf\n");
  fprintf(stdout, "hello fprintf\n");
  const char* fputsString = "hello fputs\n";
  fputs(fputsString, stdout);

  // 系统调用
  const char* wstring = "hello write\n";
  write(1, wstring, strlen(wstring));
  
  // 在代码结束前，创建子进程
  fork();

  return 0;
}
