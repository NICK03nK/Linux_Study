#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
  pid_t id = fork();
  
  if (id == 0)
  {
    // 子进程
    int cnt = 6;
    while(cnt)
    {
      printf("i am a child process, pid: %d, ppid: %d, cnt: %d\n", getpid(), getppid(), cnt--);
      sleep(1);
      int a = 10 / 0;  // 使子进程异常退出
    }
    //exit(0);  // 终止子进程
    exit(10);
  }

  // 父进程
  //sleep(9);
  //pid_t ret = wait(NULL);
  int status = 0;
  pid_t ret = waitpid(id, &status, 0);
  if (id > 0)
  {
    //printf("wait success: %d\n", ret);
    //prifntf("wait success: %d, status: %d\n", ret, status);
    printf("wait success: %d, child's exitCode: %d, sigNumber: %d\n", ret, (status >> 8) & 0XFF, status & 0X7F);
  }
  
  //sleep(3);

  return 0;
}
