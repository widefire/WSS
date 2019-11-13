#include <iostream>

#include <TCPServer.h>
#include <vector>

#include <asio.hpp>
#include "TestTCP.h"
#include "TestUDP.h"


int main(int argc, char **argv)
{
    wss::InitNetWork();
    TestUDP();
    //TestTCP();

    wss::ShutdownNetWork();
    return -1;
}