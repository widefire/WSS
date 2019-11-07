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
        static std::shared_ptr<TCPServer> Create(uint16_t port, AcceptCallback callback);
        virtual bool Start() = 0;
        virtual bool Stop() = 0;
    protected:
        uint16_t _port = 0;
        AcceptCallback _callback = nullptr;
    };


}
