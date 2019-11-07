#include "TCPServer.h"
#include "AsioTCPServer.h"

namespace wss
{

    TCPServer::TCPServer()
    {
    }


    TCPServer::~TCPServer()
    {
    }

    std::shared_ptr<TCPServer> TCPServer::Create(TCP_TYPE type,uint16_t port, AcceptCallback callback)
    {
        auto server = std::make_shared<AsioTCPServer>();

        server->_port = port;
        server->_callback = callback;
        server->_type = type;

        return server;
    }

}
