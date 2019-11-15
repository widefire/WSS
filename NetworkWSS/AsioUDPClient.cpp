#include "AsioUDPClient.h"
#include "AsioTCPClient.h"

namespace wss
{

    AsioUDPClient::AsioUDPClient(IP_ADDRESS_TYPE type, std::string remoteAddr, uint16_t remotePort)
        :
        UDPClient(type,remoteAddr,remotePort),
        _socket(GlobalAsioContext())
        ,_readDeadline(GlobalAsioContext())
    {
        std::error_code ec;

        if (IP_ADDRESS_TYPE::V4 == _ipAddrType )
        {
            _socket.open(asio::ip::udp::v4(), ec);
        }
        else
        {
            _socket.open(asio::ip::udp::v6(), ec);
        }

        asio::ip::udp::resolver resolver(GlobalAsioContext());
        _remoteEndPoint = *resolver.resolve(asio::ip::udp::v4(), _remoteAddr,std::to_string(_remotePort)).begin();

    }


    AsioUDPClient::~AsioUDPClient()
    {
        _socket.close();
        _readDeadline.cancel();
    }

    bool AsioUDPClient::Init()
    {
        _readDeadline.async_wait(std::bind(&AsioUDPClient::CheckReadDeadline, this, shared_from_this()));
        return true;
    }

    bool AsioUDPClient::Read(size_t size, NetPacket pkt, UDPCallback callback, size_t timeout)
    {
        if (timeout <= 0)
        {
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        else
        {
            _readDeadline.expires_after(std::chrono::milliseconds(timeout));
        }
        _socket.async_receive_from(
            asio::buffer(pkt->data(), pkt->size())
            , _remoteEndPoint
            , std::bind(
                &HandleRead, shared_from_this(),
                callback,
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
        return true;
    }

    bool AsioUDPClient::ReadSync(size_t size, NetPacket pkt, size_t& transferred, size_t timeout)
    {
        if (timeout <= 0)
        {
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        else
        {
            _readDeadline.expires_after(std::chrono::milliseconds(timeout));
        }
        return true;
    }
    
    bool AsioUDPClient::Write(NetPacket pkt, UDPCallback callback)
    {
        return Write(pkt->data(), pkt->size(), callback);
    }

    bool AsioUDPClient::Write(void * ptr, size_t len, UDPCallback callback)
    {
        _socket.async_send_to(
            asio::buffer(ptr, len),
            _remoteEndPoint,
            std::bind(
                &AsioUDPClient::HandleWrite,
                shared_from_this(),
                callback,
                std::placeholders::_1,
                std::placeholders::_2
            )
        );

        return true;
    }

    bool AsioUDPClient::WriteSync(NetPacket pkt,size_t& transferred)
    {
        return WriteSync(pkt->data(), pkt->size(), transferred);
    }

    bool AsioUDPClient::WriteSync(void * ptr, size_t len,size_t& transferred)
    {
        transferred = _socket.send_to(asio::buffer(ptr, len), _remoteEndPoint);
        return true;
    }

    void AsioUDPClient::HandleRead(
        std::shared_ptr<UDPClient> ptr,
        UDPCallback callback,
        const asio::error_code & error, std::size_t bytes_transferred)
    {
        auto client = std::dynamic_pointer_cast<AsioUDPClient>(ptr);
        client->_readDeadline.expires_at(asio::steady_timer::time_point::max());
        callback(error, bytes_transferred);
    }

    void AsioUDPClient::HandleWrite(
        std::shared_ptr<UDPClient> ptr,
        UDPCallback callback, 
        const asio::error_code & error, std::size_t bytes_transferred)
    {
        
        callback(error, bytes_transferred);
    }
    void AsioUDPClient::CheckReadDeadline(std::shared_ptr<UDPClient> ptr)
    {
        if (_readDeadline.expiry() <= asio::steady_timer::clock_type::now())
        {
            asio::error_code ec;
            _socket.cancel(ec);
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        _readDeadline.async_wait(std::bind(&AsioUDPClient::CheckReadDeadline, this,shared_from_this()));
    }
    std::string AsioUDPAddr::Dump()
    {
        return std::string(remoteEndpoint.address().to_string() +":"+ std::to_string(remoteEndpoint.port()));
    }
}

