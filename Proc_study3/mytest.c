#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

// 非阻塞等待
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
      //int a = 10 / 0;  // 使子进程异常退出
    }
    //exit(0);  // 终止子进程
    exit(10);
  }

  // 父进程 
  int status = 0;
  while(1)  // 非阻塞轮询
  {
    pid_t ret = waitpid(id, &status, WNOHANG);  // WNOHANG: 非阻塞等待 -> 检测到子进程没有退出，立即返回waitpid()的调用
    
    if (ret == 0)
    {
      // waitpid()调用成功 && 子进程没有退出
      // 子进程没有退出，waitpid()没有等待失败，仅仅是检测到了子进程没有退出
      printf("waitpid() call success, but child process is running...\n");
    }
    else if (ret > 0)
    {
      // waitpid()调用成功 && 子进程退出了
      printf("wait success: %d, child's exitCode: %d, sigNumber: %d\n", ret, (status >> 8) & 0XFF, status & 0X7F);
      break;
    }
    else
    {
      // waitpid()调用失败 (可能是传入waitpid()的实参id与子进程的pid不相等导致的)
      printf("waitpid() call failed\n");
      break;
    }

    sleep(1);
  }

  return 0;
}


// 阻塞等待
//int main()
//{
//  pid_t id = fork();
//  
//  if (id == 0)
//  {
//    // 子进程
//    int cnt = 6;
//    while(cnt)
//    {
//      printf("i am a child process, pid: %d, ppid: %d, cnt: %d\n", getpid(), getppid(), cnt--);
//      sleep(1);
//      //int a = 10 / 0;  // 使子进程异常退出
//    }
//    //exit(0);  // 终止子进程
//    exit(10);
//  }
//
//  // 父进程
//  //sleep(9);
//  //pid_t ret = wait(NULL);
//  int status = 0;
//  pid_t ret = waitpid(id, &status, 0);  // 0: 阻塞等待
//  if (ret > 0)
//  {
//    //printf("wait success: %d\n", ret);
//    //prifntf("wait success: %d, status: %d\n", ret, status);
//    //printf("wait success: %d, child's exitCode: %d, sigNumber: %d\n", ret, (status >> 8) & 0XFF, status & 0X7F);
//    
//    // 判断是否正常退出
//    if (WIFEXITED(status))
//    {
//      printf("exitCode: %d\n", WEXITSTATUS(status));
//    }
//  }
//  
//  //sleep(3);
//
//  return 0;
//}
