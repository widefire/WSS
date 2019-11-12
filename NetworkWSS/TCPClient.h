#pragma once

#include <functional>
#include <string>

#include "NetworkCommon.h"

namespace wss
{
    using ConnectCallback = std::function<void(const std::error_code& error)>;
    using TCPCallback = std::function<void(const std::error_code& error, std::size_t n)>;
    class TCPClient:public std::enable_shared_from_this<TCPClient>
    {
    public:
        TCPClient();
        virtual ~TCPClient();
        //必须所有的回调都完成才能释放TCP client 的内存空间
        virtual bool CanFree() = 0;
        virtual bool Stop(std::error_code& ec) = 0;
        /*!
        \param timeout millseconds,if timeout <= 0,no time out
        return true for begin connect
        */
        virtual bool Connect(size_t timeout = 0) = 0;
        /*!
        \param size targetSize for read
        \param pkt can't null,store result
        \param timeout millseconds
        return true for begin read
        */
        virtual bool Read(size_t size, NetPacket pkt, size_t timeout = 0) = 0;
        /*!
        \param pkt data to write
        return true for begin write
        */
        virtual bool Write(NetPacket pkt, size_t timeout = 0) = 0;
        /*
        write len size data;
        return true for begin write
        */
        virtual bool Write(void* ptr, size_t len, size_t timeout = 0) = 0;
        virtual bool ReadSync(size_t size, NetPacket pkt, size_t timeout = 0) = 0;
        virtual bool WriteSync(NetPacket pkt, size_t timeout = 0) = 0;
        virtual bool WriteSync(void* ptr, size_t len, size_t timeout = 0) = 0;
        static std::shared_ptr<TCPClient> Create(const std::string addr, const uint16_t port,
            ConnectCallback connCallback,
            TCPCallback readCallback,
            TCPCallback writeCallback);
        /*
        connect succeed or failed or closed 
        */
        void SetConnectCallback(ConnectCallback callback);
        void SetReadCallback(TCPCallback callback);
        void SetWriteCallback(TCPCallback callback);
        
        std::shared_ptr<TCPClient> Ptr();
        std::string RemoteAddr();
        uint16_t ReomtePort();
        TCP_TYPE TcpType();
    protected:
        std::string _addr = "localhost";
        uint16_t _port = -1;
        TCP_TYPE _tcpType = TCP_TYPE::UNKNOWN;
        ConnectCallback _connCallback = nullptr;
        TCPCallback _readCallback = nullptr;
        TCPCallback _writeCallback = nullptr;
    };


}
