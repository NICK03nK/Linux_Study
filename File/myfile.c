#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME(number) "log.txt"#number

int main()
{
  int fd0 = open(FILE_NAME(0), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd1 = open(FILE_NAME(1), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd2 = open(FILE_NAME(2), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd3 = open(FILE_NAME(3), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  int fd4 = open(FILE_NAME(4), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  
  printf("fd%d = %d\n", 0,fd0);
  printf("fd%d = %d\n", 1,fd1);
  printf("fd%d = %d\n", 2,fd2);
  printf("fd%d = %d\n", 3,fd3);
  printf("fd%d = %d\n", 4,fd4);

  close(fd0);
  close(fd1);
  close(fd2);
  close(fd3);
  close(fd4);

//  //int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
//  //int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0666);
//  //int fd = open(FILE_NAME, O_RDONLY);
//
//  if (fd < 0)
//  {
//    perror("open");
//    return 1;
//  }
//
//  char buffer[1024];
//  ssize_t num = read(fd, buffer, sizeof(buffer) - 1);
//  if (num > 0)
//  {
//    // num是从文件中读取的字节个数，buffer中的元素也是以字
//    // 节为单位的，将第num个元素置为0，以便用printf打印buffer
//    buffer[num] = 0;
//  }
//
//  printf("%s", buffer);
//
////  int cnt = 5;
////  char outBuffer[64] = { 0 };
////
////  while(cnt)
////  {
////    sprintf(outBuffer, "%s:%d\n", "This is a file", cnt--);
////
////    write(fd, outBuffer, strlen(outBuffer));
////  }
//
//  close(fd);

  return 0;
}
