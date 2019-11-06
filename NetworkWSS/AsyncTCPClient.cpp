#include "AsyncTCPClient.h"

#include "AsioAsyncTCPClient.h"

namespace wss
{

    AsyncTCPClient::AsyncTCPClient()
    {
    }


    AsyncTCPClient::~AsyncTCPClient()
    {
    }

    std::shared_ptr<AsyncTCPClient> AsyncTCPClient::Create(const std::string addr, const uint16_t port, TCP_TYPE type, TCPCallback callback)
    {
        auto client = std::make_shared<AsioAsyncTCPClient>();

        client->_addr = addr;
        client->_port = port;
        client->_tcpType = type;
        client->_tcpCallback = callback;

        return client;
    }

    std::string AsyncTCPClient::RemoteAddr()
    {
        return _addr;
    }

    uint16_t AsyncTCPClient::ReomtePort()
    {
        return _port;
    }

    TCP_TYPE AsyncTCPClient::TcpType()
    {
        return _tcpType;
    }

}
