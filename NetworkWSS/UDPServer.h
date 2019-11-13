#pragma once

#include <functional>
#include "NetworkCommon.h"
#include "UDPClient.h"

namespace wss
{
    

    using UDPReceiveCallback = std::function<
        void(
            std::shared_ptr<UDPAddr> addr,
            const std::error_code& error,
            std::size_t bytes_transferred)>;
    using UDPSendCallback = std::function<
        void(
            std::shared_ptr<UDPAddr> addr,
            const std::error_code& error,
            std::size_t bytes_transferred)>;

    class UDPServer:public std::enable_shared_from_this<UDPServer>
    {
    public:
        UDPServer(IP_ADDRESS_TYPE type, uint16_t port);
        virtual ~UDPServer();
        static std::shared_ptr<UDPServer> Create(IP_ADDRESS_TYPE type, uint16_t port);
        virtual bool Send(NetPacket pkt, std::shared_ptr<UDPAddr> addr, UDPSendCallback callback) = 0;
        virtual bool Receive(NetPacket pkt, UDPReceiveCallback callback, size_t timeout = 0) = 0;
        virtual bool SendSync(NetPacket pkt, std::shared_ptr<UDPAddr> addr, size_t& sizeSended) = 0;
        virtual bool ReceiveSync(NetPacket pkt, std::shared_ptr<UDPAddr>& addr, size_t& sizeReceived, size_t timeout = 0) = 0;
        std::shared_ptr<UDPServer> Ptr();
    protected:
        IP_ADDRESS_TYPE _type = IP_ADDRESS_TYPE::V4;
        uint16_t _port = -1;
    };


}
