#include "util.hpp"
#include <vector>
#include <functional>

#define INIT(cbs)                \
    do                           \
    {                            \
        cbs.push_back(Log);      \
        cbs.push_back(Download); \
        cbs.push_back(Sql);      \
    } while (0)

#define EXEC_OTHER(cbs)           \
    do                            \
    {                             \
        for (const auto &e : cbs) \
            e();                  \
    } while (0)

using func_t = std::function<void()>;

int main()
{
    std::vector<func_t> cbs;
    INIT(cbs);

    setNoneBlock(0);

    char buffer[1024] = { 0 };
    while (true)
    {
        std::cout << ">>> " << std::endl;
        fflush(stdout);

        ssize_t s = read(0, buffer, sizeof(buffer) - 1);  // 从0号文件描述符中最多读取sizeof(buffer) - 1字节个数据放到buffer中
        if (s > 0)
        {
            buffer[s - 1] = 0;
            std::cout << "echo# " << buffer << std::endl;
        }
        else if (s == 0)
        {
            std::cout << "read end" << std::endl;
            break;  // CRTL+D读取到文件末尾，直接退出
        }
        else
        {
            if (errno == EAGAIN)
            {
                std::cout << "no error, just no data" << std::endl;
                EXEC_OTHER(cbs);
            }
            else if (errno == EINTR)
            {
                continue;
            }
            else
            {
                std::cerr << "result: " << s << "errno: " << strerror(errno) << std::endl;
                break;
            }
        }

        sleep(1);
    }

    return 0;
}