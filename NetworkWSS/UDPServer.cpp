#include "UDPServer.h"
#include "AsioUDPServer.h"

namespace wss
{

    UDPServer::UDPServer(IP_ADDRESS_TYPE type, uint16_t port)
        :_type(type),_port(port)
    {
    }


    UDPServer::~UDPServer()
    {
    }

    std::shared_ptr<UDPServer> UDPServer::Create(IP_ADDRESS_TYPE type, uint16_t port, std::exception& ec)
    {
        auto server = std::make_shared<AsioUDPServer>(type,port);

        if (!server->Init(ec))
        {
            return nullptr;
        }

        return server;
    }
    std::shared_ptr<UDPServer> UDPServer::Ptr()
    {
        return shared_from_this();
    }
}
