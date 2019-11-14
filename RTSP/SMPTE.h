#pragma once

namespace wss
{
    //!time relative to the start of the clip
    //!The time code has the format hours:minutes:seconds:frames.subframes

    //https://blog.csdn.net/andrew57/article/details/6752182
    constexpr double SMPTE_30_drop_frame_rate = 29.97;

    class SMPTE
    {
    public:
        SMPTE();
        ~SMPTE();
    };


}
