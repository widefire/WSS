#include <iostream>

#include <TCPServer.h>
#include <vector>

#include <asio.hpp>
#include "TestTCP.h"
#include <UDPServer.h>

#define fileline __FILE__<<":"<<__LINE__<<"\t"

void UDPServerLoop()
{
    auto server = wss::UDPServer::Create(wss::IP_ADDRESS_TYPE::V4, 7080);
    const int pktSize = 100;
    auto pkt = wss::NewNetPacket(pktSize);
    auto readCallback = [&](std::shared_ptr<wss::UDPAddr> addr,
        const std::error_code& error,
        std::size_t bytes_transferred)
    {
        if (error)
        {
            std::cout << fileline << error.message() << std::endl;
        }
        else
        {
            std::cout << fileline << "received " << pkt->data() << std::endl;
        }
    };
    server->Receive(pkt, readCallback);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000000000));
}

void TestUDP()
{
    auto thSvr = std::thread(UDPServerLoop);
    
    auto client = wss::UDPClient::Create(wss::IP_ADDRESS_TYPE::V4, "localhost", 7080);
    while (true)
    {
        static const std::string buf = "hello udp";
        client->Write((void*)buf.data(), buf.size(), [&](const std::error_code& error, std::size_t bytes_transferred)
            {
                if (error)
                {
                    std::cout << fileline << error.message() << std::endl;
                }
                else
                {
                    std::cout << fileline << "send succeed" << std::endl;
                }
            });
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    thSvr.join();

    return;
}

int main(int argc, char **argv)
{
    wss::InitNetWork();
    TestUDP();
    //TestTCP();

    wss::ShutdownNetWork();
    return -1;
}