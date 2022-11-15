#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[], char* env[])
{
//// 观察argv中的内容
//
//  for (int i = 0; i < argc; ++i)
//  {
//    printf("argv[%d]->%s\n", i, argv[i]);
//  }


//// 模拟指令的选项
//
//  if (argc != 2)
//  {
//    printf("Usage: \n\t%s [-a/-b/-c/-ab/-bc/-ac/-abc]\n", argv[0]);
//    return 1;
//  }
//
//  if (strcmp("-a", argv[1]) == 0)
//  {
//    printf("功能a\n");
//  }
//  if (strcmp("-b", argv[1]) == 0)
//  {
//    printf("功能b\n");
//  }
//  if (strcmp("-c", argv[1]) == 0)
//  {
//    printf("功能c\n");
//  }
//  if (strcmp("-ab", argv[1]) == 0)
//  {
//    printf("功能ab\n");
//  }
//  if (strcmp("-ac", argv[1]) == 0)
//  {
//    printf("功能ac\n");
//  }
//  if (strcmp("-bc", argv[1]) == 0)
//  {
//    printf("功能bc\n");
//  }
//  if (strcmp("-abc", argv[1]) == 0)
//  {
//    printf("功能abc\n");
//  }


//// 环境变量列表env[]来查看环境变量
//
//  for (int i = 0; env[i]; ++i)
//  {
//    printf("env[%d]:%s\n", i, env[i]);
//  }


//// 使用extern char **environ来查看环境变量

  extern char **environ;
  for (int i = 0; environ[i]; ++i)
  {
    printf("%d:%s\n", i, environ[i]);
  }
  return 0;
}
