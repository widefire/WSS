#pragma once

#include "TCPServer.h"
#include "AsioTCPClient.h"

namespace wss
{

    class AsioTCPServer final:public TCPServer
    {
    public:
        AsioTCPServer();
        virtual ~AsioTCPServer(); 
        virtual bool Start() override;
        virtual bool Stop() override;
    private:
        bool _stoped = true;
        asio::ip::tcp::acceptor _acceptor;
    private:
        void Accept();
    };


}
