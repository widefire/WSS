#pragma once

#include <functional>
#include <string>

#include "NetworkCommon.h"

namespace wss
{
    using TCPCallback = std::function<void(const std::error_code& error)>;
    class AsyncTCPClient
    {
    public:
        AsyncTCPClient();
        virtual ~AsyncTCPClient();
        /*!
        \param timeOut millseconds,if timeOut <= 0,no time out
        return true for begin connect
        */
        virtual bool Connect(size_t timeOut = 0) = 0;
        /*!
        \param size targetSize for read
        \param pkt can't null,store result
        \param timeOut millseconds
        return true for begin read
        */
        virtual bool Read(size_t size, NetPacket pkt, size_t timeOut = 0) = 0;
        /*!
        \param pkt data to write
        return true for begin write
        */
        virtual bool Write(NetPacket pkt, size_t timeOut = 0) = 0;
        /*
        write len size data;
        return true for begin write
        */
        virtual bool Write(void* ptr, size_t len, size_t timeOut = 0) = 0;
        static std::shared_ptr<AsyncTCPClient> Create(const std::string addr, const uint16_t port, TCP_TYPE type, TCPCallback callback);
        
        std::string RemoteAddr();
        uint16_t ReomtePort();
        TCP_TYPE TcpType();
    protected:
        std::string _addr = "localhost";
        uint16_t _port = -1;
        TCP_TYPE _tcpType = TCP_TYPE::V4;
        TCPCallback _tcpCallback = nullptr;
    };


}
