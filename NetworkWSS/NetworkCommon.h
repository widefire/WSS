#pragma once

#include <memory>
#include <vector>

namespace wss
{
    enum class TCP_TYPE
    {
        V4,
        V6
    };

    using NetPacket = std::shared_ptr<std::vector<uint8_t>>;
    inline NetPacket NewNetPacket() { return std::make_shared<std::vector<uint8_t>>(); }
    inline NetPacket NewNetPacket(size_t size) { return std::make_shared<std::vector<uint8_t>>(size, 0); }
}
