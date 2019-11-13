#pragma once
#include "UDPServer.h"
#include "AsioUDPClient.h"
#include <asio.hpp>

namespace wss
{

    class AsioUDPServer final:
        public UDPServer
    {
    public:
        AsioUDPServer(IP_ADDRESS_TYPE type, uint16_t port);
        ~AsioUDPServer();
        bool Init(std::exception& ec);
        virtual bool Receive(NetPacket pkt, UDPReceiveCallback callback, size_t timeout = 0) override;
        virtual bool Send(NetPacket pkt, std::shared_ptr<UDPAddr> addr, UDPSendCallback callback) override;
        virtual bool Send(void* data, size_t len, std::shared_ptr<UDPAddr> addr, UDPSendCallback callback)override;
        virtual bool ReceiveSync(NetPacket pkt, std::shared_ptr<UDPAddr>& addr, size_t& sizeReceived,size_t timeout = 0) override;
        virtual bool SendSync(NetPacket pkt, std::shared_ptr<UDPAddr> addr, size_t& sizeSended) override;
    private:
        asio::ip::udp::socket _socket;
        asio::steady_timer _readDeadline;
    private:
        static void HandleReceive(
            std::shared_ptr<UDPServer> ptr,
            std::shared_ptr<UDPAddr> addr,
            UDPReceiveCallback callback,
            const asio::error_code& error,
            std::size_t bytes_transferred);
        static void HandleSend
        (
            std::shared_ptr<UDPServer> ptr,
            std::shared_ptr<UDPAddr> addr,
            UDPSendCallback callback,
            const asio::error_code& error,
            std::size_t bytes_transferred
        );

        static void CheckReadDeadline(std::shared_ptr<UDPServer> ptr);
    };

}

