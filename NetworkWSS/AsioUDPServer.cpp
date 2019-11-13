#include "AsioUDPServer.h"
#include "AsioTCPClient.h"

namespace wss
{

    AsioUDPServer::AsioUDPServer(IP_ADDRESS_TYPE type, uint16_t port)
        :UDPServer(type,port),
        _socket(GlobalAsioContext(), asio::ip::udp::endpoint(asio::ip::udp::v4(),port))
        ,_readDeadline(GlobalAsioContext())
    {
        _readDeadline.async_wait(std::bind(&AsioUDPServer::CheckReadDeadline, this));
    }


    AsioUDPServer::~AsioUDPServer()
    {
        _readDeadline.cancel();
        _socket.close();
    }

    bool AsioUDPServer::Receive(NetPacket pkt, UDPReceiveCallback callback, size_t timeout)
    {
        if (!_socket.is_open())
        {
            return false;
        }

        if (timeout <= 0)
        {
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        else
        {
            _readDeadline.expires_after(std::chrono::milliseconds(timeout));
        }

        auto addr = std::make_shared<AsioUDPAddr>();
        _socket.async_receive_from(
            asio::buffer(pkt->data(), pkt->size()),
            addr->remoteEndpoint,
            std::bind(&AsioUDPServer::HandleReceive,
                Ptr(),
                addr,
                callback,
                std::placeholders::_1,
                std::placeholders::_2
            ));

        return true;
    }

    bool AsioUDPServer::ReceiveSync(NetPacket pkt, std::shared_ptr<UDPAddr>& addr, size_t& sizeReceived,size_t timeout)
    {
        if (!_socket.is_open())
        {
            return false;
        }

        auto asioAddr = std::dynamic_pointer_cast<AsioUDPAddr>(addr);
        if (asioAddr == nullptr)
        {
            return false;
        }

        if (timeout <= 0)
        {
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        else
        {
            _readDeadline.expires_after(std::chrono::milliseconds(timeout));
        }

        sizeReceived = _socket.receive_from(asio::buffer(pkt->data(), pkt->size()), asioAddr->remoteEndpoint);

        return true;
    }

    bool AsioUDPServer::Send(NetPacket pkt, std::shared_ptr<UDPAddr> addr, UDPSendCallback callback)
    {
        if (!_socket.is_open())
        {
            return false;
        }

        auto asioAddr = std::dynamic_pointer_cast<AsioUDPAddr>(addr);
        if (asioAddr == nullptr)
        {
            return false;
        }

        _socket.async_send_to(
            asio::buffer(pkt->data(), pkt->size()),
            asioAddr->remoteEndpoint,
            std::bind(
                &AsioUDPServer::HandleSend,
                Ptr(),
                addr,
                callback,
                std::placeholders::_1,
                std::placeholders::_2)
        );

        return true;
    }

    bool AsioUDPServer::SendSync(NetPacket pkt, std::shared_ptr<UDPAddr> addr, size_t& sizeSended)
    {
        if (!_socket.is_open())
        {
            return false;
        }

        auto asioAddr = std::dynamic_pointer_cast<AsioUDPAddr>(addr);
        if (asioAddr == nullptr)
        {
            return false;
        }

        sizeSended = _socket.send_to(asio::buffer(pkt->data(), pkt->size()), asioAddr->remoteEndpoint);

        return true;
    }

    void AsioUDPServer::HandleReceive(
        std::shared_ptr<UDPServer> ptr,
        std::shared_ptr<UDPAddr> addr, 
        UDPReceiveCallback callback,
        const asio::error_code & error, std::size_t bytes_transferred)
    {
        auto client = std::dynamic_pointer_cast<AsioUDPServer>(ptr);
        client->_readDeadline.expires_at(asio::steady_timer::time_point::max());
        callback(addr, error, bytes_transferred);
    }

    void AsioUDPServer::HandleSend(std::shared_ptr<UDPServer> ptr, std::shared_ptr<UDPAddr> addr, UDPSendCallback callback, const asio::error_code& error, std::size_t bytes_transferred)
    {
        callback(addr, error, bytes_transferred);
    }

    void AsioUDPServer::CheckReadDeadline()
    {
        if (_readDeadline.expiry() <= asio::steady_timer::clock_type::now())
        {
            asio::error_code ec;
            _socket.cancel(ec);
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        _readDeadline.async_wait(std::bind(&AsioUDPServer::CheckReadDeadline, this));
    }

}