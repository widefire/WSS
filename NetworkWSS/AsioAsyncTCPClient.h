#pragma once

#include <asio.hpp>

#include "AsyncTCPClient.h"

namespace wss
{
    //read_some ��һ��������
    //read ���ǳ������������
    asio::io_context& GlobalTCPClientContext();
    class AsioAsyncTCPClient final :public AsyncTCPClient
    {
    public:
        AsioAsyncTCPClient();
        virtual ~AsioAsyncTCPClient();
        virtual bool Connect(size_t timeOut = 0) override;
        virtual bool Read(size_t size, NetPacket pkt, size_t timeOut = 0) override;
        virtual bool Write(NetPacket pkt, size_t timeOut = 0) override;
        virtual bool Write(void* ptr, size_t len, size_t timeOut = 0) override;
        
    };
}


