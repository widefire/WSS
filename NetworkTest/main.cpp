#include <iostream>

#include <TCPServer.h>

int main(int argc, char **argv)
{
    wss::InitNetWork();
    auto server = wss::TCPServer::Create(wss::TCP_TYPE::V4, 7080, nullptr);
    auto ret = server->Start();

    while (true)
    {
        std::string line;
        std::getline(std::cin, line);
    }

    return -1;
}