#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER "USER"
#define MY_ENV "myval"
#define MY_PWD "PWD"

int main()
{
//// 获取用户名
//
//  char* whoami = getenv(USER);
//  printf("user:%s\n", who);


//// 查看当前用户是否有root权限
//
//  char* who = getenv(USER);
//  if (strcmp(who, "root") == 0)
//  {
//    printf("user:%s\n", who);
//  }
//  else
//  {
//   printf("without permission\n");
//  }


//// 判断该变量是否为环境变量
//
//  char* myenv = getenv(MY_ENV);
//  if (myenv == NULL)
//  {
//    printf("%s:is no found\n", MY_ENV);
//    return 1;
//  }
//  else
//  {
//    printf("%s=%s\n", MY_ENV, myenv);
//  }

//// 模拟pwd指令
//
  printf("%s\n", getenv(MY_PWD));

  return 0;
}
