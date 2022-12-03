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
  fp->capacity = SIZE;
  memset(fp->buffer, 0, SIZE);

  return fp;  // 最后将fp返回，对应了C中fopen打开文件成功后的返回值为FILE*指针
}

void fwrite_(const void* ptr, int num, FILE_* fp)
{
  // 1. 将数据先写入缓冲区
  memcpy(fp->buffer + fp->size, ptr, num);  // fp->buffer + fp->size的作用是：若buffer缓冲区中还有未刷新的数据，则接着数据后面继续写入
  fp->size += num;

  // 2. 判断是否刷新数据
  if (fp->flags & SYNC_NOW)  // 无缓冲
  {
    write(fp->fileno, fp->buffer, fp->size);
    fp->size = 0;  // 起清空缓冲区的作用，下一次再将数据写入缓冲区时，则是从头开始覆盖写入
  }
  else if (fp->flags & SYNC_FULL)  // 全缓冲
  {
    if (fp->size == fp->capacity)
    {
      write(fp->fileno, fp->buffer, fp->size);
      fp->size = 0;
    }
  }
  else if (fp->flags & SYNC_LINE)  // 行缓冲
  {
    if (fp->buffer[fp->size - 1] == '\n')  // 该判断条件满足行缓冲的要求   注：此Demo程序不考虑如：abc\ndef等复杂情况
    {
      write(fp->fileno, fp->buffer, fp->size);
      fp->size = 0;
    }
  }
}

void fflush_(FILE_* fp)
{
  if (fp->size > 0)
  {
    write(fp->fileno, fp->buffer, fp->size);
  }

  fsync(fp->fileno);  // 系统调用，强制要求OS将缓冲区上的数据刷新到外设

  fp->size = 0;
}

void fclose_(FILE_* fp)
{
  fflush_(fp);

  close(fp->fileno);
}
