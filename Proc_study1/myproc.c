#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
  // 调用fork()函数创建子进程，在fork()函数执行前：只有一个父进程，fork()函数执行后：父进程 + 子进程
  pid_t id = fork();
  // 成功创建子进程，将子进程的pid作为fork()函数的返回值返回给父进程，将0作为fork()函数的返回值返回给
  // 子进程；若创建子进程失败，则将-1返回给父进程，不创建子进程，并且设置错误码。

  if (id == 0)
  {
    // 子进程
    while (1)
    {
       printf("子进程, pid : %d, ppid : %d, id : %d\n", getpid(), getppid(), id);
       sleep(1);
    }

  }
  else if (id > 0)
  {
    // 父进程
    while (1)
    {
       printf("父进程, pid : %d, ppid : %d, id : %d\n", getpid(), getppid(), id);
       sleep(2);
    }
  }
  else
  {
    // error
  }

  return 0;
}

