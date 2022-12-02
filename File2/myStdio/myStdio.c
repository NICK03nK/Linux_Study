#include "myStdio.h"

FILE_* fopen_(const char* pathName, const char* mode)
{
  // 设置打开文件的flags选项
  int flags = 0;
  if (strcmp(mode, "r") == 0)
  {
    flags |= O_RDONLY;
  }
  else if (strcmp(mode, "w") == 0)
  {
    flags |= (O_WRONLY | O_CREAT | O_TRUNC);
  }
  else if (strcmp(mode, "a") == 0)
  {
    flags |= (O_WRONLY | O_CREAT | O_APPEND);
  }
  else
  {
    // do nothing
  }
  
  // 设置好flags选项后，根据flags选项来进行打开文件
  int fd = 0;
  int defaultMode = 0666;
  if (flags & O_RDONLY)
  {
    fd = open(pathName, flags);
  }
  else
  {
    fd = open(pathName, flags, defaultMode);
  }
  
  if (fd < 0)
  {
    const char* err = strerror(errno);
    write(2, err, strlen(err));
    return NULL;  // 此处解释了为什么C中的fopen打开文件失败返回NULL
  }
  
  // 文件打开成功，申请对应的空间
  FILE_* fp = (FILE_*)malloc(sizeof(FILE_));
  assert(fp);
  
  // 对fp进行初始化
  fp->flags = SYNC_LINE;  // 刷新方式默认设置为行刷新
  fp->fileno = fd;
  fp->size = 0;
  fp->capcity = SIZE;
  memset(fp->buffer, 0, SIZE);

  return fp;  // 最后将fp返回，对应了C中fopen打开文件成功后的返回值为FILE*指针
}

int fwrite_(void* ptr, int num, FILE_* fp);

void fclose_(FILE_* fp);
