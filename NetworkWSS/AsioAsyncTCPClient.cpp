#include "AsioAsyncTCPClient.h"

using asio::ip::tcp;
namespace wss
{

    AsioAsyncTCPClient::AsioAsyncTCPClient()
        :
        _socket(GlobalTCPClientContext())
        ,_readDeadline(GlobalTCPClientContext())
        ,_writeDeadline(GlobalTCPClientContext())
    {
    }


    AsioAsyncTCPClient::~AsioAsyncTCPClient()
    {
    }

    bool AsioAsyncTCPClient::Connect(size_t timeout)
    {
        _readTimeout = timeout;
        tcp::resolver r(GlobalTCPClientContext());
        std::error_code ec;

        _endpoints = r.resolve(_addr, std::to_string(_port), ec);
        if (ec)
        {
            return false;
        }

        asio::ip::address addr;
        addr = addr.from_string(_addr);
        if (addr.is_v4())
        {
            _tcpType = TCP_TYPE::V4;
        }
        else if (addr.is_v6())
        {
            _tcpType = TCP_TYPE::V6;
        }

        _notifyedConnectStatus = false;
        StartConnect(_endpoints.begin());

        _readDeadline.async_wait(std::bind(&AsioAsyncTCPClient::CheckReadDeadline, this));

        return true;
    }

    bool AsioAsyncTCPClient::Read(size_t size, NetPacket pkt, size_t timeout)
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

        _readTimeout = timeout;
        if (_readTimeout <= 0)
        {
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        else
        {
            _readDeadline.expires_after(std::chrono::milliseconds(_readTimeout));
        }

        asio::async_read(_socket, asio::buffer(pkt->data(), pkt->size()), 
            [&](const std::error_code& error, std::size_t n)
            {
                if (_stoped)
                {
                    return;
                }
                _readDeadline.expires_at(asio::steady_timer::time_point::max());
                _readCallback(error, n);
                if (!error)
                {
                }
                else
                {
                    Stop();
                }
            });

        return true;
    }

    bool AsioAsyncTCPClient::Write(NetPacket pkt, size_t timeout)
    {
        return false;
    }

    bool AsioAsyncTCPClient::Write(void * ptr, size_t len, size_t timeout)
    {
        return false;
    }

    void AsioAsyncTCPClient::Stop()
    {
        _stoped = true;
        std::error_code ec;
        _socket.close(ec);
        _readDeadline.cancel();
        _writeDeadline.cancel();
    }

    void AsioAsyncTCPClient::StartConnect(tcp::resolver::results_type::iterator endpointIter)
    {
        if (endpointIter!=_endpoints.end())
        {
            if (_readTimeout<=0)
            {
                _readDeadline.expires_at(asio::steady_timer::time_point::max());
            }
            else
            {
                _readDeadline.expires_after(std::chrono::milliseconds(_readTimeout));
            }
            _socket.async_connect(endpointIter->endpoint(),
                std::bind(&AsioAsyncTCPClient::HandleConnect, this, std::placeholders::_1, endpointIter));
        }
        else
        {
            Stop();
            NotifyConnectStatus(false);
        }
    }

    void AsioAsyncTCPClient::HandleConnect(const std::error_code & error, tcp::resolver::results_type::iterator endpointIter)
    {
        if (_stoped)
        {
            NotifyConnectStatus(false);
            return;
        }

        if (!_socket.is_open())
        {
            StartConnect(++endpointIter);
        }
        else if (error)
        {
            _socket.close();
            StartConnect(++endpointIter);
        }
        else
        {
            NotifyConnectStatus(true);
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
    }

    void AsioAsyncTCPClient::CheckReadDeadline()
    {
        if (_stoped)
        {
            return;
        }

        if (_readDeadline.expiry()<=asio::steady_timer::clock_type::now())
        {
            asio::error_code ec;
            _socket.close(ec);
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        _readDeadline.async_wait(std::bind(&AsioAsyncTCPClient::CheckReadDeadline, this));
    }

    void AsioAsyncTCPClient::NotifyConnectStatus(bool succeed)
    {
        if (!_notifyedConnectStatus)
        {
            _notifyedConnectStatus = true;
            if (succeed)
            {
                std::error_code ec;
                _connCallback(ec);
            }
            else
            {
                _connCallback(std::make_error_code(std::errc::connection_aborted));
            }
        }
    }

    asio::io_context & GlobalTCPClientContext()
    {
        static asio::io_context ctx;
        return ctx;
    }

}
