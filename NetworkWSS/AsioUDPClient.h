#pragma once
#include <asio.hpp>
#include "UDPClient.h"

namespace wss
{

    class AsioUDPAddr :public UDPAddr
    {
    public:
        asio::ip::udp::endpoint remoteEndpoint;
        virtual std::string Dump() override;
    };

    class AsioUDPClient :
        public UDPClient
    {
    public:
        AsioUDPClient(IP_ADDRESS_TYPE type, std::string remoteAddr, uint16_t remotePort);
        ~AsioUDPClient();
        bool Init();
        virtual bool Read(size_t size, NetPacket pkt, UDPCallback callback, size_t timeout = 0)override;
        virtual bool ReadSync(size_t size, NetPacket pkt, size_t& transferred, size_t timeout = 0)override;
        virtual bool Write(NetPacket pkt, UDPCallback callback)override;
        virtual bool Write(void* ptr, size_t len, UDPCallback callback)override;
        virtual bool WriteSync(NetPacket pkt,size_t& transferred )override;
        virtual bool WriteSync(void* ptr, size_t len,size_t& transferred)override;
    private:
        asio::ip::udp::socket _socket;
        asio::ip::udp::endpoint _localEndPoint;
        asio::ip::udp::endpoint _remoteEndPoint;
        asio::steady_timer _readDeadline;
    private:
        static void HandleRead(std::shared_ptr<UDPClient> ptr, UDPCallback callback, const asio::error_code& error, std::size_t bytes_transferred);
        static void HandleWrite(std::shared_ptr<UDPClient> ptr, UDPCallback callback, const asio::error_code& error, std::size_t bytes_transferred);
        void CheckReadDeadline(std::shared_ptr<UDPClient> ptr);
    };


}
