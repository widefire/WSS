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
        if (nullptr != _connCallback)
        {
            _connCallback(std::make_error_code(std::errc::connection_aborted));
        }

        
        std::lock_guard<std::mutex> guard(_mutexAsyncCount);
        if (0 != _asyncCount)
        {
            std::cout << __FILE__ << " " << __LINE__ << ":destory before all async callback end,dangerous";
        }
        //std::unique_lock<std::mutex> lk(_mutexAsyncCount);
        //_conAsyncCount.wait(lk, [&]() {return _asyncReadCount == 0; });
    }

    bool AsioTCPClient::CanFree()
    {

        std::lock_guard<std::mutex> guard(_mutexAsyncCount);
        return _asyncCount == 0;
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

    bool AsioTCPClient::Connect(size_t timeout)
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
        StartConnect(_endpoints.begin());

        _readDeadline.async_wait(std::bind(&AsioTCPClient::CheckReadDeadline, this));
        _writeDeadline.async_wait(std::bind(&AsioTCPClient::CheckWriteDeadline, this));

        return true;
    }

    bool AsioTCPClient::Read(size_t size, NetPacket pkt, size_t timeout)
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

        IncreaseAsyncCount();
        asio::async_read(_socket, asio::buffer(pkt->data(), pkt->size()), 
            [&](const std::error_code& error, std::size_t n)
            {
                _readDeadline.expires_at(asio::steady_timer::time_point::max());
                _readCallback(error, n);

                if (_stoped)
                {
                    DecreaseAsyncCount();
                    return;
                }
                if (error)
                {
                    std::error_code ec;
                    Stop(ec);
                }
                DecreaseAsyncCount();
            });

        return true;
    }

    bool AsioTCPClient::Write(NetPacket pkt, size_t timeout)
    {
        return Write(static_cast<void*>(pkt->data()), pkt->size(), timeout);

    }

    bool AsioTCPClient::Write(void * ptr, size_t len, size_t timeout)
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
        IncreaseAsyncCount();


        /*asio::async_write(_socket, asio::buffer(ptr, len),
            [&](const asio::error_code& error, std::size_t bytes_transferred)
            {
                _writeDeadline.expires_at(asio::steady_timer::time_point::max());
                _writeCallback(error, bytes_transferred);
                if (_stoped)
                {
                    DecreaseAsyncCount();
                    return;
                }
                if (error)
                {
                    std::error_code ec;
                    Stop(ec);
                }
                DecreaseAsyncCount();
            });*/

        asio::async_write(_socket, 
            asio::buffer(ptr, len),
            std::bind(&AsioTCPClient::AsyncWrite, this, Ptr(),
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


    void AsioTCPClient::StartConnect(tcp::resolver::results_type::iterator endpointIter)
    {
        if (endpointIter!=_endpoints.end())
        {
            UpdateReadDeadline(_readTimeout);
            UpdateWriteDeadline(0);
            IncreaseAsyncCount();
            _socket.async_connect(endpointIter->endpoint(),
                std::bind(&AsioTCPClient::HandleConnect, this, std::placeholders::_1, endpointIter));
        }
        else
        {
            std::error_code ec;
            Stop(ec);
            NotifyConnectStatus(false);
        }
    }

    void AsioTCPClient::HandleConnect(const std::error_code & error, tcp::resolver::results_type::iterator endpointIter)
    {
        if (_stoped)
        {
            NotifyConnectStatus(false);
            DecreaseAsyncCount();
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
            
            if (endpointIter->endpoint().address().is_v4())
            {
                _tcpType = TCP_TYPE::V4;
            }
            else if (endpointIter->endpoint().address().is_v6())
            {
                _tcpType = TCP_TYPE::V6;
            }
            else
            {
                _tcpType = TCP_TYPE::UNKNOWN;
            }
            NotifyConnectStatus(true);
            _readDeadline.expires_at(asio::steady_timer::time_point::max());
        }
        DecreaseAsyncCount();
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

    void AsioTCPClient::NotifyConnectStatus(bool succeed)
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

    void AsioTCPClient::DecreaseAsyncCount()
    {
        std::lock_guard<std::mutex> guard(_mutexAsyncCount);
        _asyncCount--;
        if (_asyncCount==0)
        {
            _conAsyncCount.notify_one();
        }
    }

    void AsioTCPClient::IncreaseAsyncCount()
    {
        std::lock_guard<std::mutex> guard(_mutexAsyncCount);
        _asyncCount++;
    }

    void AsioTCPClient::AsyncWrite(std::shared_ptr<TCPClient> client, const asio::error_code & error, std::size_t bytes_transferred)
    {
        _writeDeadline.expires_at(asio::steady_timer::time_point::max());
        _writeCallback(error, bytes_transferred);
        if (_stoped)
        {
            DecreaseAsyncCount();
            return;
        }
        if (error)
        {
            std::error_code ec;
            Stop(ec);
        }
        DecreaseAsyncCount();
    }


}
