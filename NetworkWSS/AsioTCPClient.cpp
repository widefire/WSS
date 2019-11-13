#include "AsioTCPClient.h"
#include <iostream>

using asio::ip::tcp;
namespace wss
{

    AsioTCPClient::AsioTCPClient()
        :
        _socket(GlobalAsioContext())
        ,_readDeadline(GlobalAsioContext())
        ,_writeDeadline(GlobalAsioContext())
    {
    }

    AsioTCPClient::AsioTCPClient(asio::ip::tcp::socket socket)
        :
        _socket(std::move(socket))
        , _readDeadline(GlobalAsioContext())
        , _writeDeadline(GlobalAsioContext())
        , _stoped(false)
    {
    }


    AsioTCPClient::~AsioTCPClient()
    {
        std::error_code ec;
        Stop(ec);
    }


    bool AsioTCPClient::Stop(std::error_code & ec)
    {
        if (_stoped)
        {
            return true;
        }
        _stoped = true;
        //std::error_code ec;

        _socket.cancel(ec);
        _socket.shutdown(asio::ip::tcp::socket::shutdown_both);
        _socket.close(ec);
        _readDeadline.cancel();
        _writeDeadline.cancel();
        if (ec)
        {
            return false;
        }
        return true;
    }

    bool AsioTCPClient::Connect(ConnectCallback callback, size_t timeout)
    {

        if (!_stoped)
        {
            return false;
        }
        _stoped = false;
        _readTimeout = timeout;
        tcp::resolver r(GlobalAsioContext());
        std::error_code ec;

        _endpoints = r.resolve(_addr, std::to_string(_port), ec);
        if (ec)
        {
            return false;
        }

        _notifyedConnectStatus = false;
        StartConnect(callback, _endpoints.begin());

        _readDeadline.async_wait(std::bind(&AsioTCPClient::CheckReadDeadline, this));
        _writeDeadline.async_wait(std::bind(&AsioTCPClient::CheckWriteDeadline, this));

        return true;
    }

    bool AsioTCPClient::Read(size_t size, NetPacket pkt, TCPCallback callback, size_t timeout)
    {
        if (_stoped)
        {
            return false;
        }

        if (nullptr == pkt)
        {
            return false;
        }

        if (pkt->size()!=size)
        {
            pkt->resize(size, 0);
        }

        UpdateReadDeadline(timeout);

        asio::async_read(_socket, asio::buffer(pkt->data(), pkt->size()),
            std::bind(&AsioTCPClient::HandleRead, 
                Ptr(),callback,
                std::placeholders::_1, std::placeholders::_2));

        return true;
    }

    bool AsioTCPClient::Write(NetPacket pkt, TCPCallback callback, size_t timeout)
    {
        return Write(static_cast<void*>(pkt->data()), pkt->size(),callback, timeout);

    }

    bool AsioTCPClient::Write(void * ptr, size_t len, TCPCallback callback, size_t timeout)
    {
        if (_stoped || ptr == nullptr)
        {
            return false;
        }

        if (len == 0)
        {
            return true;
        }

        UpdateWriteDeadline(timeout);


        asio::async_write(_socket, 
            asio::buffer(ptr, len),
            std::bind(&AsioTCPClient::HandleWrite, Ptr(),callback,
            std::placeholders::_1, std::placeholders::_2));

        return true;
    }

    bool AsioTCPClient::ReadSync(size_t size, NetPacket pkt, size_t timeout)
    {
        if (_stoped||pkt==nullptr)
        {
            return false;
        }

        if (pkt->size()!=size)
        {
            pkt->resize(size);
        }

        UpdateReadDeadline(timeout);

        size_t readed = 0;
        while (readed<size)
        {
            std::error_code ec;

            auto ret = asio::read(_socket, asio::buffer(pkt->data() + readed, pkt->size() - readed),ec);
            if (ec)
            {
                UpdateReadDeadline(0);
                Stop(ec);
                return false;
            }
            readed += ret;
        }

        UpdateReadDeadline(0);
        return true;
    }

    bool AsioTCPClient::WriteSync(NetPacket pkt, size_t timeout)
    {
        return WriteSync(pkt->data(), pkt->size(), timeout);
    }

    bool AsioTCPClient::WriteSync(void * ptr, size_t len, size_t timeout)
    {
        if (_stoped||nullptr==ptr)
        {
            return false;
        }

        if (len == 0)
        {
            return true;
        }

        UpdateWriteDeadline(timeout);

        std::error_code ec;

        size_t writed = 0;
        while (writed < len)
        {
            auto ret = asio::write(_socket, asio::buffer((char*)ptr+writed, len-writed), ec);
            if (ec)
            {
                UpdateWriteDeadline(0);
                Stop(ec);
                return false;
            }
            writed += ret;
        }
        UpdateWriteDeadline(0);
        return true;
    }


    void AsioTCPClient::StartConnect(ConnectCallback callback, tcp::resolver::results_type::iterator endpointIter)
    {
        if (endpointIter!=_endpoints.end())
        {
            UpdateReadDeadline(_readTimeout);
            UpdateWriteDeadline(0);
            _socket.async_connect(endpointIter->endpoint(),
                std::bind(&AsioTCPClient::HandleConnect, 
                    Ptr(),callback,
                    std::placeholders::_1, endpointIter));
        }
        else
        {
            std::error_code ec;
            Stop(ec);
            NotifyConnectStatus(callback,false);
        }
    }

    void AsioTCPClient::HandleConnect(std::shared_ptr<TCPClient> ptr,
        ConnectCallback callback,
        const std::error_code & error, tcp::resolver::results_type::iterator endpointIter)
    {
        auto client = std::dynamic_pointer_cast<AsioTCPClient>(ptr);
        if (client->_stoped)
        {
            client->NotifyConnectStatus(callback, false);
            return;
        }

        if (!client->_socket.is_open())
        {
            client->StartConnect(callback, ++endpointIter);
        }
        else if (error)
        {
            client->_socket.close();
            client->StartConnect(callback, ++endpointIter);
        }
        else
        {
            
            if (endpointIter->endpoint().address().is_v4())
            {
                client->_tcpType = IP_ADDRESS_TYPE::V4;
            }
            else if (endpointIter->endpoint().address().is_v6())
            {
                client->_tcpType = IP_ADDRESS_TYPE::V6;
            }
            else
            {
                client->_tcpType = IP_ADDRESS_TYPE::UNKNOWN;
            }
            client->NotifyConnectStatus(callback, true);
            client->_readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
    }

    void AsioTCPClient::UpdateReadDeadline(const size_t timeout)
    {
        _readTimeout = timeout;
        if (_readTimeout <= 0)
        {
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        else
        {
            _readDeadline.expires_after(std::chrono::milliseconds(_readTimeout));
        }
    }

    void AsioTCPClient::UpdateWriteDeadline(const size_t timeout)
    {
        _writeTimeout = timeout;
        if (_writeTimeout<=0)
        {
            _writeDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        else
        {
            _writeDeadline.expires_after(std::chrono::milliseconds(_writeTimeout));
        }

    }

    void AsioTCPClient::CheckReadDeadline()
    {
        if (_stoped)
        {
            return;
        }

        auto expiry = _readDeadline.expiry();
        auto now = asio::steady_timer::clock_type::now();
        if (_readDeadline.expiry()<=asio::steady_timer::clock_type::now())
        {
            asio::error_code ec;
            _socket.close(ec);
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        _readDeadline.async_wait(std::bind(&AsioTCPClient::CheckReadDeadline, this));
    }

    void AsioTCPClient::CheckWriteDeadline()
    {
        if (_stoped)
        {
            return;
        }
        auto expiry = _writeDeadline.expiry();
        auto now = asio::steady_timer::clock_type::now();
        if (_writeDeadline.expiry()<=asio::steady_timer::clock_type::now())
        {
            asio::error_code ec;
            _socket.close(ec);
            _writeDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        _writeDeadline.async_wait(std::bind(&AsioTCPClient::CheckWriteDeadline, this));
    }

    void AsioTCPClient::NotifyConnectStatus(ConnectCallback callback, bool succeed)
    {
        if (!_notifyedConnectStatus)
        {
            _notifyedConnectStatus = true;
            if (succeed)
            {
                std::error_code ec;
                callback(ec);
            }
            else
            {
                callback(std::make_error_code(std::errc::connection_aborted));
            }
        }
    }

    void AsioTCPClient::HandleWrite(std::shared_ptr<TCPClient> ptr,
        TCPCallback callback,
        const asio::error_code & error, std::size_t bytes_transferred)
    {
        auto client = std::dynamic_pointer_cast<AsioTCPClient>(ptr);
        client->_writeDeadline.expires_at(asio::steady_timer::time_point::max());
        callback(error, bytes_transferred);
        if (client->_stoped)
        {
            return;
        }
        if (error)
        {
            std::error_code ec;
            client->Stop(ec);
        }
    }

    void AsioTCPClient::HandleRead(std::shared_ptr<TCPClient> ptr, 
        TCPCallback callback,
        const asio::error_code & error, std::size_t bytes_transferred)
    {
        auto client = std::dynamic_pointer_cast<AsioTCPClient>(ptr);
        client->_readDeadline.expires_at(asio::steady_timer::time_point::max());
        callback(error, bytes_transferred);

        if (client->_stoped)
        {
            return;
        }
        if (error)
        {
            std::error_code ec;
            client->Stop(ec);
        }
    }


}
