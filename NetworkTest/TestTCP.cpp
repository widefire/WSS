#include "TestTCP.h"
#include <iostream>

#include <TCPServer.h>
#include <vector>

#include <asio.hpp>

#define fileline __FILE__<<":"<<__LINE__<<"\t"


std::vector<std::shared_ptr<wss::TCPClient>> accepted;
void WriteTest(std::shared_ptr<wss::TCPClient> client)
{
    static std::string helloStr = "welcome connect";
    if (!client->Write(const_cast<char*>(helloStr.c_str()), helloStr.size(),
        [&](const std::error_code& error, std::size_t n)
        {
            if (error)
            {
                std::cout << fileline << error.message() << std::endl;
            }
            else
            {
                std::cout << fileline << " write succeed" << std::endl;
                WriteTest(accepted[0]);
            }
        }
    ))
    {
        std::cout << fileline << "write failed" << std::endl;
    }
}

void TestTCP()
{
    const uint16_t port = 7080;

    auto server = wss::TCPServer::Create(wss::IP_ADDRESS_TYPE::V4, port, [&](std::shared_ptr<wss::TCPClient> client)
        {
            accepted.push_back(client);
            std::cout << fileline << "accept client" << std::endl;
            WriteTest(client);
        });

    auto ret = server->Start();

    
        auto server2 = wss::TCPServer::Create(wss::IP_ADDRESS_TYPE::V4, port, [&](std::shared_ptr<wss::TCPClient> client)
            {
                accepted.push_back(client);
                std::cout << fileline << "accept client" << std::endl;
                WriteTest(client);
            });

        auto ret2 = server2->Start();
    
        server = nullptr;
    

    {
        wss::NetPacket pkt = wss::NewNetPacket();
        std::shared_ptr<wss::TCPClient> client;
        client = wss::TCPClient::Create("localhost", port);

        if (!client->Connect([&](const std::error_code& error)
            {
                if (error)
                {
                    std::cout << fileline << error.message() << std::endl;
                }
                else
                {
                    std::cout << fileline << "connect succeed" << std::endl;
                    client->Read(100, pkt, [&](const std::error_code& errorCode, std::size_t n)
                        {
                            if (errorCode)
                            {
                                if (n > 0)
                                {
                                    std::cout << pkt->data() << std::endl;
                                }
                                std::cout << fileline << "read error:" << errorCode.message() << std::endl;
                            }
                            else
                            {
                                std::cout << fileline << "read ok" << std::endl;
                            }
                        });
                }
            })
            )
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

    for (auto &it : accepted)
    {
        it = nullptr;
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
