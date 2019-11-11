#pragma once

#include "TCPClient.h"

namespace wss
{
    using AcceptCallback = std::function<void(std::shared_ptr<TCPClient> client)>;
    class TCPServer
    {
    public:
        TCPServer();
        virtual ~TCPServer();
        static std::shared_ptr<TCPServer> Create(
            TCP_TYPE type,uint16_t port, 
            AcceptCallback callback,
            TCPCallback defaultReadCallback,
            TCPCallback defaultWriteCallback);
        virtual bool Start() = 0;
        virtual bool Stop() = 0;
    protected:
        uint16_t _port = 0;
        AcceptCallback _callback = nullptr;
        TCPCallback _defaultReadCallback = nullptr;
        TCPCallback _defaultWriteCallback = nullptr;
        TCP_TYPE _type = TCP_TYPE::UNKNOWN;
    };


}
