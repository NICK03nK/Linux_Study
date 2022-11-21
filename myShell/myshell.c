#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define NUM 1024
#define OPT_NUM 64

char lineCommand[NUM];
char* myargv[OPT_NUM];

int main()
{
  while(1)
  {
    // 若在提示符中直接输出PWD，显示效果与shell的命令行提示符不相符
    // 所以这里的处理是为了拿到PWD的最后一个/后的路径
    char* myPWD = NULL;
    char tmp[100] = { 0 };
    strcpy(tmp, getenv("PWD"));
    for (int i = strlen(tmp) - 1; i >= 0; --i)
    {
      if (tmp[i] == '/')
      {
        myPWD = &tmp[i + 1];
        break;
      }
    }

    // 输出命令行提示符
    printf("(%s@%s %s)$ ", getenv("USER"), getenv("HOSTNAME"), myPWD);
    fflush(stdout);

    // 获取用户输入的命令
    char* s = fgets(lineCommand, sizeof(lineCommand) - 1, stdin);
    assert(s != NULL);
    (void)s;
    // 清除输入指令时从键盘输入的\n   abcd\n   len = 5
    lineCommand[strlen(lineCommand) - 1] = '\0';

    // 分割用户输入的指令，存入myargv中
    myargv[0] = strtok(lineCommand, " ");
    // 当lineCommand中没有" "时，strtok()返回NULL，而myargv最后要以NULL结尾，所以一下实现方式可完美契合
    int i = 1;
    while(myargv[i++] = strtok(NULL, " "));

    // 创建子进程来执行用户输入的命令
    pid_t id = fork();
    assert(id != -1);
    
    if (id == 0)
    {
      // 子进程
      // 执行命令
      execvp(myargv[0], myargv);
      exit(1);
    }
    
    // 父进程
    waitpid(id, NULL, 0);
  }

  return 0;
}
