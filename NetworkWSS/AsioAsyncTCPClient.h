#pragma once

#include <asio.hpp>
#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/steady_timer.hpp>
#include <asio/write.hpp>

#include "AsyncTCPClient.h"

namespace wss
{
    //read_some 不一定读完整
    //read 除非出错，否则读完整
    asio::io_context& GlobalTCPClientContext();
    class AsioAsyncTCPClient final :public AsyncTCPClient
    {
    public:
        AsioAsyncTCPClient();
        virtual ~AsioAsyncTCPClient();
        virtual bool Connect(size_t timeout = 0) override;
        virtual bool Read(size_t size, NetPacket pkt, size_t timeout = 0) override;
        virtual bool Write(NetPacket pkt, size_t timeout = 0) override;
        virtual bool Write(void* ptr, size_t len, size_t timeout = 0) override;
    private:
        bool _stoped = false;
        asio::ip::tcp::resolver::results_type _endpoints;
        asio::ip::tcp::socket   _socket;
        asio::steady_timer _readDeadline;//and for connect
        asio::steady_timer _writeDeadline;
        size_t _readTimeout = 0;
        size_t _writeTimeout = 0;
        bool _notifyedConnectStatus = false;
    private:
        void Stop();
        void StartConnect(asio::ip::tcp::resolver::results_type::iterator endpointIter);
        void HandleConnect(const std::error_code& error,asio::ip::tcp::resolver::results_type::iterator endpointIter);
        void CheckReadDeadline();
        void NotifyConnectStatus(bool succeed);
    };
}


