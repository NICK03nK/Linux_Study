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
int lastCode = 0;

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
    int i = 1;

    // 若输入的指令为ls，则加上颜色选项
    if (myargv[0] && strcmp(myargv[0], "ls") == 0)
    {
      myargv[i++] = (char*)"--color=auto";
    }

    // 当lineCommand中没有" "时，strtok()返回NULL，而myargv最后要以NULL结尾，所以一下实现方式可完美契合
    while(myargv[i++] = strtok(NULL, " "));
    
    // 如果输入的命令是cd，则不需要创建子进程，而是让父进程继续执行后续的命令 --- 内建命令
    if (myargv[0] && strcmp(myargv[0], "cd") == 0)
    {
      if (myargv[1])
      {
        chdir(myargv[1]);
      }

      continue;
    }
    
    // 如果输入的命令是echo，则也是一个内建命令，由父进程自己执行即可
    if (myargv[0] && strcmp(myargv[0], "echo") == 0)
    {
      if (strcmp(myargv[1], "$?") == 0)
      {
        printf("%d\n", lastCode);
        // 使用echo输出完上一个进程的退出码后要将lastCode置为0，因为echo也是一个进程 
        lastCode = 0; 
      }
      else
      {
        printf("%s\n", myargv[1]);
      }

      continue;
    }

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
    int status = 0;
    pid_t ret = waitpid(id, &status, 0);

    assert(ret > 0);
    (void)ret;

    lastCode = (status >> 8) & 0xFF;
  }

  return 0;
}
