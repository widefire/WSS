#include <iostream>

#include <TCPServer.h>
#include <vector>

#define fileline __FILE__<<":"<<__LINE__<<"\t"

int main(int argc, char **argv)
{
    wss::InitNetWork();

    const uint16_t port = 7080;

    std::vector<std::shared_ptr<wss::TCPClient>> accepted;
    auto server = wss::TCPServer::Create(wss::TCP_TYPE::V4, port, [&](std::shared_ptr<wss::TCPClient> client)
        {
            //accepted.push_back(client);
            //auto ptr = client->Ptr();
            std::cout << fileline << "accept client" << std::endl;
            client->SetConnectCallback([](const std::error_code &ec)
                {
                    std::cout << fileline << "connect closed :"<<ec.message() << std::endl;
                });
            std::string helloStr = "welcome connect";
            if (!client->Write(const_cast<char*>(helloStr.c_str()), helloStr.size()))
            {
                std::cout << fileline << "write failed" << std::endl;
            }
        }, 
        [](const std::error_code& errorCode, std::size_t n)
        {
            if (errorCode)
            {
                std::cout << fileline << "read error:" << errorCode.message() << std::endl;
            }
            else
            {
                std::cout << fileline << "read ok" << std::endl;
            }
        },
        [](const std::error_code& errorCode, std::size_t n)
        {
            if (errorCode)
            {
                std::cout << fileline << "write error:" << errorCode.message() << std::endl;
            }
            else
            {
                std::cout << fileline << "write ok" << std::endl;
            }
        });
    auto ret = server->Start();

    {
        wss::NetPacket pkt = wss::NewNetPacket();
        std::shared_ptr<wss::TCPClient> client;
        client = wss::TCPClient::Create("localhost", port,
            [&](const std::error_code& errorCode)
            {
                if (errorCode)
                {
                    std::cout << fileline << "connect error:" << errorCode.message() << std::endl;
                }
                else
                {
                    std::cout << fileline << "connect ok" << std::endl;
                    client->Read(100, pkt);
                }
            },
            [&](const std::error_code& errorCode, std::size_t n)
            {
                if (errorCode)
                {
                    if (n>0)
                    {
                        std::cout << pkt->data() << std::endl;
                    }
                    std::cout << fileline << "read error:" << errorCode.message() << std::endl;
                }
                else
                {
                    std::cout << fileline << "read ok" << std::endl;
                }
            },
                [](const std::error_code& errorCode, std::size_t n)
            {
                if (errorCode)
                {
                    std::cout << fileline << "write error:" << errorCode.message() << std::endl;
                }
                else
                {
                    std::cout << fileline << "write ok" << std::endl;
                }
            }
            );

        if (!client->Connect())
        {
            std::cout << fileline << "connect error" << std::endl;
        }

        while (true)
        {
            std::string line;
            std::getline(std::cin, line);
            if (line.compare("q") == 0)
            {
                break;
            }
        }
    }
        
    for (auto &it:accepted)
    {
        it = nullptr;
    }
        while (true)
        {
            std::string line;
            std::getline(std::cin, line);
            if (line.compare("q")==0)
            {
                break;
            }
        }

        wss::ShutdownNetWork();
    return -1;
}