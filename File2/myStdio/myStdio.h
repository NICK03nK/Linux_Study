#pragma once

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define SIZE 1024
#define SYNC_NOW 1  // 无缓冲
#define SYNC_LINE 2  // 行缓冲
#define SYNC_FULL 4  // 全缓冲

typedef struct _FILE
{
  int flags;  // 刷新方式
  int fileno;
  int size;  // buffer的当前使用量
  int capacity;  // buffer的总容量
  char buffer[SIZE];  // 用户级语言层面的缓冲区
}FILE_;

FILE_* fopen_(const char* pathName, const char* mode);

void fwrite_(const void* ptr, int num, FILE_* fp);

void fflush_(FILE_* fp);

void fclose_(FILE_* fp);
