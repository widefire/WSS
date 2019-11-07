#pragma once

#include <asio.hpp>
#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/steady_timer.hpp>
#include <asio/write.hpp>

#include "TCPClient.h"

namespace wss
{

    asio::io_context& GlobalAsioContext();
    class AsioTCPClient final :public TCPClient
    {
    public:
        AsioTCPClient();
        AsioTCPClient(asio::ip::tcp::socket socket);
        virtual ~AsioTCPClient();
        virtual bool Connect(size_t timeout = 0) override;
        virtual bool Read(size_t size, NetPacket pkt, size_t timeout = 0) override;
        virtual bool Write(NetPacket pkt, size_t timeout = 0) override;
        virtual bool Write(void* ptr, size_t len, size_t timeout = 0) override;
        virtual bool ReadSync(size_t size, NetPacket pkt, size_t timeout = 0) override;
        virtual bool WriteSync(NetPacket pkt, size_t timeout = 0) override;
        virtual bool WriteSync(void* ptr, size_t len, size_t timeout = 0) override;
    private:
        bool _stoped = true;
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
        void UpdateReadDeadline(const size_t timeout);
        void UpdateWriteDeadline(const size_t timeout);
        void CheckReadDeadline();
        void CheckWriteDeadline();
        void NotifyConnectStatus(bool succeed);
    };
}


