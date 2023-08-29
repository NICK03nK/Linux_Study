#pragma once

#include <iostream>
#include <string>
#include <cstdarg>
#include <ctime>
#include <cstdio>

using namespace std;

#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

#define NUM 1024

const char *toLevelStr(int level)
{
    switch (level)
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

void logMessage(int level, const char *format, ...)
{
    char logPrefix[NUM] = {0};
    snprintf(logPrefix, sizeof(logPrefix), "[%s] [%ld]", toLevelStr(level), (long int)time(nullptr));

    char logContent[NUM] = {0};
    va_list arg;
    va_start(arg, format);
    vsnprintf(logContent, sizeof(logContent), format, arg);

    cout << logPrefix << logContent << endl;
}