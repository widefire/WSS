#include "NetworkCommon.h"

#include "AsioTCPClient.h"

namespace wss
{
    asio::io_context & GlobalAsioContext()
    {
        static asio::io_context ctx;
        return ctx;
    }
    asio::io_context::work worker(GlobalAsioContext());
    std::thread workerThread;

    void InitNetWork()
    {
        workerThread = std::thread([&]() {GlobalAsioContext().run(); });
    }

    void ShutdownNetWork()
    {
        auto& ctx = GlobalAsioContext();
        ctx.stop();
        workerThread.join();
    }

}
