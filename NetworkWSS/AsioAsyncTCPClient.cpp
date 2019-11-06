#include "AsioAsyncTCPClient.h"

namespace wss
{

    AsioAsyncTCPClient::AsioAsyncTCPClient()
    {
    }


    AsioAsyncTCPClient::~AsioAsyncTCPClient()
    {
    }

    asio::io_context & GlobalTCPClientContext()
    {
        static asio::io_context ctx;
        return ctx;
    }

}
