#pragma once

#include <iostream>
#include <string>
#include <cstdarg>
#include <ctime>
#include <cstdio>

using namespace std;

#define DEBUG   0
#define NORMAL  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

#define NUM 1024

#define LOG_NORMAL "log.txt"
#define LOG_ERR "log.error"

const char* toLevelStr(int level)
{
    switch(level)
    {
    case DEBUG:
        return "DEBUG";
    case NORMAL:
        return "NORMAL";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    default:
        return nullptr;
    }
}

// [日志等级] [时间戳] [content]
void logMessage(int level, const char* format, ...)
{
    char logPrefix[NUM] = { 0 };
    snprintf(logPrefix, sizeof(logPrefix), "[%s] [%ld]", toLevelStr(level), (long int)time(nullptr));

    char logContent[NUM] = { 0 };
    va_list arg;
    va_start(arg, format);
    vsnprintf(logContent, sizeof(logContent), format, arg);

    FILE* log = fopen(LOG_NORMAL, "a");
    FILE* err = fopen(LOG_ERR, "a");

    if(log != nullptr && err != nullptr)
    {
        FILE *curr = nullptr;
        if(level == DEBUG || level == NORMAL || level == WARNING) curr = log;
        if(level == ERROR || level == FATAL) curr = err;
        if(curr) fprintf(curr, "%s%s\n", logPrefix, logContent);

        fclose(log);
        fclose(err);
    }
}