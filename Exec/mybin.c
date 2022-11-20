#include <stdio.h>
#include <stdlib.h>

int main()
{
  // 系统自带的环境变量 
  printf("PATH: %s\n", getenv("PATH"));
  printf("PWD: %s\n", getenv("PWD"));
  // 用户创建的环境变量
  printf("myEnv: %s\n", getenv("myEnv"));

  printf("Test program replacement\n");
  printf("Test program replacement\n");
  printf("Test program replacement\n");
  printf("Test program replacement\n");
  printf("Test program replacement\n");

  return 0;
}
