#include "AsioTCPServer.h"


using asio::ip::tcp;
namespace wss
{

    AsioTCPServer::AsioTCPServer()
        :
        _acceptor(GlobalAsioContext())
    {
    }


    AsioTCPServer::~AsioTCPServer()
    {
    }

    bool AsioTCPServer::Start()
    {
        if (!_stoped)
        {
            return false;
        }

        std::error_code ec;

        tcp::endpoint listen_endpoint;

        switch (_type)
        {
        case wss::TCP_TYPE::UNKNOWN:
            return false;
            break;
        case wss::TCP_TYPE::V4:
            listen_endpoint = tcp::endpoint(tcp::v4(), _port);
            break;
        case wss::TCP_TYPE::V6:
            listen_endpoint = tcp::endpoint(tcp::v6(), _port);
            break;
        default:
            return false;
            break;
        }

        _acceptor.open(listen_endpoint.protocol(), ec);
        if (ec)
        {
            return false;
        }
        _acceptor.bind(listen_endpoint, ec);
        if (ec)
        {
            return false;
        }

        _stoped = false;

        Accept();
        

        return true;
    }

    bool AsioTCPServer::Stop()
    {
        if (_stoped)
        {
            return true;
        }

        std::error_code ec;

        _acceptor.close(ec);
        _stoped = true;
        return true;
    }

    void AsioTCPServer::Accept()
    {

        _acceptor.async_accept([&](const std::error_code& error, tcp::socket socket)
            {
                if (!error)
                {
                    auto client = std::make_shared<AsioTCPClient>(std::move(socket));
                    _callback(client);
                }
                if (!_stoped)
                {
                    Accept();
                }
            });
    }

}
