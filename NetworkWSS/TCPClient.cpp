#include "TCPClient.h"

#include "AsioTCPClient.h"

namespace wss
{

    TCPClient::TCPClient()
    {
    }


    TCPClient::~TCPClient()
    {
    }

    std::shared_ptr<TCPClient> TCPClient::Create(const std::string addr, const uint16_t port)
    {
        auto client = std::make_shared<AsioTCPClient>();

        client->_addr = addr;
        client->_port = port;

        return client;
    }


    std::shared_ptr<TCPClient> TCPClient::Ptr()
    {
        return shared_from_this();
    }

    std::string TCPClient::RemoteAddr()
    {
        return _addr;
    }

    uint16_t TCPClient::ReomtePort()
    {
        return _port;
    }

    IP_ADDRESS_TYPE TCPClient::TcpType()
    {
        return _tcpType;
    }

}
