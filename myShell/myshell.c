#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define NUM 1024
#define OPT_NUM 64

#define NONE_REDIR 0
#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPEND_REDIR 3

#define trimSpace(start) do{\
  while (isspace(*start)) ++start;\
}while(0)

char lineCommand[NUM];
char* myargv[OPT_NUM];
int lastCode = 0;

int redirType = NONE_REDIR;
char* redirFile = NULL;

void commandCheak(char* commands)
{
  assert(commands);

  char* start = commands;
  char* end = commands + strlen(commands);
  
  while(start < end)
  {
    if (*start == '>')
    {
      *start = '\0';
      ++start;
      
      // 判断是输出重定向还是追加重定向
      if (*start == '>')
      {
        redirType = APPEND_REDIR;
        ++start;
      }
      else
      {
        redirType = OUTPUT_REDIR;
      }

      trimSpace(start);
      redirFile = start;

      break;
    }
    else if (*start == '<')
    {
      *start = '\0';
      ++start;
      trimSpace(start);

      // 填写重定向的信息
      redirType = INPUT_REDIR;
      redirFile = start;

      break;
    }
    else
    {
      ++start;
    }
  }

}

void inputRedirFunc()
{
  int fd = open(redirFile, O_RDONLY);
  if (fd < 0)
  {
    perror("open");
    exit(errno);
  }
  
  // 重定向的文件已成功打开
  dup2(fd, 0);
}

void output_appendRedirFunc()
{
  umask(0);
  int flags = O_WRONLY | O_CREAT;
  if (redirType == APPEND_REDIR)
  {
    flags |= O_APPEND;
  }
  else
  {
    flags |= O_TRUNC;
  }

  int fd = open(redirFile, flags, 0666);
  if (fd < 0)
  {
    perror("open");
    exit(errno);
  }

  // 重定向的文件已成功打开
  dup2(fd, 1);
}

int main()
{
  while(1)
  {
    // 初始化重定向的两个变量
    redirType = NONE_REDIR;
    redirFile = NULL;
    errno = 0;

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
  
    // 重定向操作的实现：
    // "ls -a -l > myfile.txt" --> "ls -a -l" > "myfile.txt"
    // "ls -a -l >> myfile.txt" --> "ls -a -l" >> "myfile.txt"
    // "cat < myfile.txt" --> "cat" < "myfile.txt"
    commandCheak(lineCommand);
   
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
      // 由子进程来执行重定向的操作
      
      switch(redirType)
      {
        case NONE_REDIR:
          // 无重定向，无需执行重定向操作
          break;
        case INPUT_REDIR:
          inputRedirFunc();
          break;
        case OUTPUT_REDIR:
        case APPEND_REDIR:
          output_appendRedirFunc();
          break;
        default:
          printf("BUGS!\n");
          break;
      }
      
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

