#pragma once

#include <iostream>
#include <functional>
#include "NetworkCommon.h"

namespace wss
{
    class UDPAddr
    {
    public:
        UDPAddr() = default;
        virtual ~UDPAddr() = default;
        virtual std::string Dump() = 0;
    };

    using UDPCallback = std::function<void(const std::error_code& error, std::size_t bytes_transferred)>;

    class UDPClient:public std::enable_shared_from_this<UDPClient>
    {
    public:
        UDPClient(IP_ADDRESS_TYPE type, std::string remoteAddr, uint16_t remotePort);
        virtual ~UDPClient();
        static std::shared_ptr<UDPClient> Create(
            IP_ADDRESS_TYPE type, std::string remoteAddr, uint16_t remotePort
        );
        virtual bool Read(size_t size, NetPacket pkt, UDPCallback callback, size_t timeout = 0) = 0;
        virtual bool Write(NetPacket pkt, UDPCallback callback) = 0;
        virtual bool Write(void* ptr, size_t len, UDPCallback callback) = 0;
        virtual bool ReadSync(size_t size, NetPacket pkt,size_t& transferred, size_t timeout = 0) = 0;
        virtual bool WriteSync(NetPacket pkt,size_t& transferred ) = 0;
        virtual bool WriteSync(void* ptr, size_t len,size_t& transferred ) = 0;
    protected:
        IP_ADDRESS_TYPE _ipAddrType = IP_ADDRESS_TYPE::V4;
        uint16_t _localPort = -1;
        uint16_t _remotePort = -1;
        std::string _remoteAddr = "";
    };


}
