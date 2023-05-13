#include <iostream>
#include <string>

using namespace std;

#define DEBUG   0
#define NORMAL  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

void logMessage(int level, const string& message)
{
    // [日志等级] [时间] [pid] [message]
    cout << message << endl;
}