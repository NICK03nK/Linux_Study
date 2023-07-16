#include "util.hpp"

int main()
{
    char buffer[1024] = { 0 };
    while (true)
    {
        std::cout << ">>> ";
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
            // ?
        }
    }

    return 0;
}