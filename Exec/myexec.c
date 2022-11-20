#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
  printf("process is running...\n");

  pid_t id = fork();
  assert(id != -1);
  
  if (id == 0)
  {
    sleep(1);

    execvp(argv[1], &argv[1]);
    
    exit(1);
  }

  int status = 0;
  pid_t ret = waitpid(id, &status, 0);

  if (ret > 0)
  {
    printf("wait success: %d, child's exitCode: %d, sigNumber: %d\n", ret, (status >> 8) & 0XFF, status & 0X7F); 
  }

  return 0;
}


//int main()
//{
//  printf("process is running...\n");
//
//  pid_t id = fork();
//  assert(id != -1);
//  
//  if (id == 0)
//  {
//    sleep(1);
//    //execl("/usr/bin/ls", "ls", "--color=auto", "-a", "-l", NULL);
//    
//    //execlp("ls", "ls", "--color=auto", "-a", "-l", NULL);
//    
//    //char* const _argv[] = {"ls", "--color=auto", "-a", "-l", NULL};
//    //execv("/usr/bin/ls", _argv);
//    
//    //execvp("ls", _argv);
//    
//    // 将当前进程替换为mybin程序
//    //execl("./mybin", "./mybin", NULL);
//    
//    // 将当前C进程替换为CPP进程mycpp
//    //execl("./mycpp", "./mycpp", NULL);
//    
//    //char* const _envp[] = {(char*)"myEnv=1122", NULL};
//    //execle("./mybin", "./mybin", NULL, _envp);  // 自定义环境变量
//    
//    extern char** environ;
//    putenv((char*)"myEnv=1122");  // 将用户创建的环境变量导入系统中，即导入environ指向的环境变量表中
//    execle("./mybin", "./mybin", NULL, environ);
//
//    // 当调用execl()失败后，执行exit，将子进程的退出码设置为1
//    exit(1);
//  }
//
//  int status = 0;
//  pid_t ret = waitpid(id, &status, 0);
//
//  if (ret > 0)
//  {
//    printf("wait success: %d, child's exitCode: %d, sigNumber: %d\n", ret, (status >> 8) & 0XFF, status & 0X7F); 
//  }
//
//  return 0;
//}


// 一般不这样写，而是采用创建子进程的方式，让子进程来执行程序替换的操作
//int main()
//{
//  printf("process is running...\n");
//
//  //int ret = execl("/usr/bin/ls", "ls", "--color=auto", "-a", "-l", NULL);
//  
//  // 当前execl()调用失败，调用失败则不会覆盖当前进程的代码与数据，所以继续执行第二个printf()
//  int ret = execl("/usr/bin/lsasdkjadal", "ls", "--color=auto", "-a", "-l", NULL);
//  
//  if (ret == -1)
//  {
//    perror("execl");
//
//    printf("process running end...\n");
//
//    exit(-1);
//  }
//
//  // printf()没有被执行是因为，调用成功execl()后，execl()后的代码就被覆盖了，
//  // 开始执行新的代码，所以printf()无法执行
//
//  return 0;
//}
