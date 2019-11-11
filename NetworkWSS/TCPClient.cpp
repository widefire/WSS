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

    std::shared_ptr<TCPClient> TCPClient::Create(const std::string addr, const uint16_t port,
        ConnectCallback connCallback,
        TCPCallback readCallback,
        TCPCallback writeCallback)
    {
        auto client = std::make_shared<AsioTCPClient>();

        client->_addr = addr;
        client->_port = port;
        client->_connCallback = connCallback;
        client->_readCallback = readCallback;
        client->_writeCallback = writeCallback;

        return client;
    }

    void TCPClient::SetConnectCallback(ConnectCallback callback)
    {
        _connCallback = callback;
    }

    void TCPClient::SetReadCallback(TCPCallback callback)
    {
        _readCallback = callback;
    }

    void TCPClient::SetWriteCallback(TCPCallback callback)
    {
        _writeCallback = callback;
    }

    std::string TCPClient::RemoteAddr()
    {
        return _addr;
    }

    uint16_t TCPClient::ReomtePort()
    {
        return _port;
    }

    TCP_TYPE TCPClient::TcpType()
    {
        return _tcpType;
    }

}
