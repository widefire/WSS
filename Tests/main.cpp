#include <iostream>

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING 1
#include <gtest/gtest.h>
#include <NetworkCommon.h>
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    wss::InitNetWork();
    RUN_ALL_TESTS();
    //wss::ShutdownNetWork();
    return 0;
}