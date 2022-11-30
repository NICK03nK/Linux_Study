#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
  //close(0);
  //close(1);
  //close(2);

  umask(0);
  //int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);  // 输出重定向
  //int fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);  // 追加重定向
  int fd = open("log.txt", O_RDONLY);  // 输入重定向
 
  if (fd < 0)
  {
    perror("open");
    return 1;
  }
  
  // 输入重定向
  dup2(fd, 0);

  char line[64] = { 0 };
  while(1)
  {
    printf(">> ");
    if (fgets(line, sizeof(line), stdin) == NULL)
    {
      break;
    }
    printf("%s", line);
  }

  close(fd);

    // 输出重定向&追加重定向
//  dup2(fd, 1);
//
//  printf("open fd:%d\n", fd);
//
//  const char* buffer = "test\n";
//  write(1, buffer, strlen(buffer));
//  
//  fflush(stdout);
//
//  close(fd);

  return 0;
}
