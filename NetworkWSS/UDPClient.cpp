#include "UDPClient.h"
#include "AsioUDPClient.h"

namespace wss
{
    UDPClient::UDPClient(IP_ADDRESS_TYPE type, std::string remoteAddr, uint16_t remotePort)
        :_ipAddrType(type)
        ,_remoteAddr(remoteAddr)
        ,_remotePort(remotePort)
    {
    }

    UDPClient::~UDPClient()
    {
    }

    std::shared_ptr<UDPClient> UDPClient::Create(
        IP_ADDRESS_TYPE type, 
        std::string remoteAddr, uint16_t remotePort
    )
    {
        auto client = std::make_shared<AsioUDPClient>(type, remoteAddr, remotePort);
        if (!client->Init())
        {
            return nullptr;
        }
        return client;
    }
}

