#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING 1
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <thread>
#include <UDPServer.h>
#include <array>

#define fileline __FILE__<<":"<<__LINE__<<"\t"

std::array<bool, 2> readed = { false,false };

std::shared_ptr<wss::UDPServer> udpServer;
const int pktSize = 100;
auto pkt = wss::NewNetPacket(pktSize);

constexpr int waitMill = 1000 * 5;
void UDPServerReadCallback(
    std::shared_ptr<wss::UDPAddr> addr,
    const std::error_code& error,
    std::size_t bytes_transferred);

void UDPServerWriteCallback(
    std::shared_ptr<wss::UDPAddr> addr,
    const std::error_code& error,
    std::size_t bytes_transferred
)
{
    udpServer->Send(pkt, addr, [&](std::shared_ptr<wss::UDPAddr> addr,
        const std::error_code& error,
        std::size_t bytes_transferred)
        {
            if (error)
            {
                std::cout << error.message() << std::endl;
            }
            else
            {
                std::cout << fileline << "server send succeed" << std::endl;
                udpServer->Receive(pkt, UDPServerReadCallback);
            }
        });
}

void UDPServerReadCallback(
    std::shared_ptr<wss::UDPAddr> addr,
    const std::error_code& error,
    std::size_t bytes_transferred)
{
    if (error)
    {
        std::cout << fileline << error.message() << std::endl;
    }
    else
    {
        auto strAddr = addr->Dump();
        std::cout << fileline << "serverd received " << pkt->data() << std::endl;
        static const std::string svrMsg = "hello ,I'm server";
        udpServer->Send((void*)svrMsg.data(), svrMsg.size(), addr, UDPServerWriteCallback);
        readed[0] = true;
    }
}

void UDPServerLoop()
{
    std::exception ec;
    udpServer = wss::UDPServer::Create(wss::IP_ADDRESS_TYPE::V4, 7080, ec);
    try
    {
        auto udpServer2 = wss::UDPServer::Create(wss::IP_ADDRESS_TYPE::V4, 7081, ec);
    }
    catch (const std::exception& exp)
    {
        std::cout << exp.what() << std::endl;
    }

    udpServer->Receive(pkt, UDPServerReadCallback);

    std::this_thread::sleep_for(std::chrono::milliseconds(waitMill));
}

std::shared_ptr<wss::UDPClient> client = nullptr;
const int clientPktSize = 100;
auto clientPkt = wss::NewNetPacket(clientPktSize);

static const std::string buf = "hello udp,I'm client";
void ClientWriteCallback(const std::error_code& error, std::size_t bytes_transferred);
void ClientReadCallback(const std::error_code& error, std::size_t bytes_transferred)
{
    if (error)
    {
        std::cout << fileline << error.message() << std::endl;
    }
    else
    {
        std::cout << fileline << "client readed " << clientPkt->data() << std::endl;
        client->Write((void*)buf.data(), buf.size(), ClientWriteCallback);
        readed[1] = true;
    }
}

void ClientWriteCallback(const std::error_code& error, std::size_t bytes_transferred)
{
    if (error)
    {
        std::cout << fileline << error.message() << std::endl;
    }
    else
    {
        std::cout << fileline << "client send succeed" << std::endl;
        client->Read(clientPktSize, clientPkt, ClientReadCallback);
    }
}

void TestUDP()
{
    auto thSvr = std::thread(UDPServerLoop);

    client = wss::UDPClient::Create(wss::IP_ADDRESS_TYPE::V4, "localhost", 7080);
    client->Write((void*)buf.data(), buf.size(), ClientWriteCallback);
    std::this_thread::sleep_for(std::chrono::milliseconds(waitMill));

    thSvr.join();

    return;
}


class UDPTest : public testing::Test {
protected:
};

TEST(UDPTest, tcp_base_test)
{
    if (true)
    {
        TestUDP();
        ASSERT_TRUE(readed[0]);
        ASSERT_TRUE(readed[1]);
    }
}