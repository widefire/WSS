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

    std::shared_ptr<AsyncTCPClient> AsyncTCPClient::Create(const std::string addr, const uint16_t port,
        ConnectCallback connCallback,
        TCPCallback readCallback,
        TCPCallback writeCallback)
    {
        auto client = std::make_shared<AsioAsyncTCPClient>();

        client->_addr = addr;
        client->_port = port;
        client->_connCallback = connCallback;
        client->_readCallback = readCallback;
        client->_writeCallback = writeCallback;

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
