#include "httpServer.hpp"
#include <memory>

using namespace std;
using namespace Server;

void Usage(string proc)
{
    cerr << "Usage:\n\t" << proc << " port\r\n\r\n";
}

string suffixToDesc(const string& suffix)
{
    string ct = "Content-Type: ";

    if (suffix == ".html")
    {
        ct += "text/html";
    }
    else if (suffix == ".jpg")
    {
        ct += "application/x-jpg";
    }

    ct += "\r\n";

    return ct;
}

bool Get(const httpRequest& req, httpResponse& resp)
{
    // for test
    cout << "-------------------http request start-------------------" << endl;
    cout << req.inbuffer << endl;

    // 打印请求行的三个字段
    cout << "method: " << req.method << endl;
    cout << "url: " << req.url << endl;
    cout << "httpVersion: " << req.httpVersion << endl;

    cout << "path: " << req.path << endl;
    cout << "suffix: " << req.suffix << endl;
    cout << "size: " << req.size << " byte" << endl;
    cout << "--------------------http request end--------------------" << endl;

    string respLine = "HTTP/1.1 200 OK\r\n";
    string respHeader = suffixToDesc(req.suffix);

    // if (req.size > 0)
    // {
    //     respHeader += "Content-Length: ";
    //     respHeader += to_string(req.size);
    //     respHeader += "\r\n";
    // }

    respHeader += "Set-Cookie: 18124665842@\r\n";

    string respBlank = "\r\n";

    string body;
    if (!util::readFile(req.path, &body))  // 要访问的资源路径不存在
    {
        util::readFile(html_404, &body);  // 将404作为访问的资源
    }

    resp.outbuffer += respLine;
    resp.outbuffer += respHeader;
    resp.outbuffer += respBlank;
    resp.outbuffer += body;

    cout << "-------------------http response start-------------------" << endl;
    cout << resp.outbuffer << endl;
    cout << "--------------------http response end--------------------" << endl;


    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port = atoi(argv[1]);

    unique_ptr<httpServer> httpSvr(new httpServer(port, Get));
    httpSvr->initServer();
    httpSvr->start();

    return 0;
}