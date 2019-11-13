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
        void Init();
        virtual bool Stop(std::error_code& ec) override;
        virtual bool Connect(ConnectCallback callback, size_t timeout = 0) override;
        virtual bool Read(size_t size, NetPacket pkt, TCPCallback callback, size_t timeout = 0) override;
        virtual bool Write(NetPacket pkt, TCPCallback callback, size_t timeout = 0) override;
        virtual bool Write(void* ptr, size_t len, TCPCallback callback, size_t timeout = 0) override;
        virtual bool ReadSync(size_t size, NetPacket pkt, size_t timeout = 0) override;
        virtual bool WriteSync(NetPacket pkt, size_t timeout = 0) override;
        virtual bool WriteSync(void* ptr, size_t len, size_t timeout = 0) override;
    private:
        bool _stoped = true;
        asio::ip::tcp::resolver::results_type _endpoints;
        asio::ip::tcp::socket   _socket;
        asio::steady_timer _readDeadline;
        asio::steady_timer _writeDeadline;
        size_t _readTimeout = 0;
        size_t _writeTimeout = 0;
        bool _notifyedConnectStatus = false;
    private:
        void StartConnect(ConnectCallback callback, asio::ip::tcp::resolver::results_type::iterator endpointIter);
        static void HandleConnect(std::shared_ptr<TCPClient> client, ConnectCallback callback, const std::error_code& error,asio::ip::tcp::resolver::results_type::iterator endpointIter);
        static void HandleWrite(std::shared_ptr<TCPClient> client, TCPCallback callback, const asio::error_code& error, std::size_t bytes_transferred);
        static void HandleRead(std::shared_ptr<TCPClient> client, TCPCallback callback, const asio::error_code& error, std::size_t bytes_transferred);
        void UpdateReadDeadline(const size_t timeout);
        void UpdateWriteDeadline(const size_t timeout);
        static void CheckReadDeadline(std::shared_ptr<TCPClient> ptr);
        static void CheckWriteDeadline(std::shared_ptr<TCPClient> ptr);
        void NotifyConnectStatus(ConnectCallback callback, bool succeed);
    };
}


