#pragma once

#include <iostream>
#include <array>
#include <vector>

namespace wss
{
    inline const uint8_t FlvHeaderSignature[] = { 'F','L','V' };

    class FLVTagData
    {
    public:
        FLVTagData() = default;
        virtual ~FLVTagData() = default;
    };

    class FLVTag final
    {
    public:
        FLVTag() = default;
        ~FLVTag() = default;

        enum class TagType : uint8_t
        {
            audio=8,
            video=9,
            scrip_data=18
        };

        TagType     tagType = TagType::audio;
        uint32_t    dataSize = 0;
        uint32_t    timestamp = 0;
        uint32_t    streamID = 0;
        std::shared_ptr<FLVTagData> Data;
    };


    class FLVAudioTag :public FLVTagData
    {
    public:
        FLVAudioTag() = default;
        ~FLVAudioTag() = default;
        enum class SoundFormat :uint8_t
        {
            SoundFormat_LinearPCM_platformEndian = 0,
            SoundFormat_ADPCM = 1,
            SoundFormat_MP3 = 2,
            SoundFormat_LinearPCM_littleEndian = 3,
            SoundFormat_Nellymoser16KHzMono = 4,
            SoundFormat_Nellymoser8KHzMono = 5,
            SoundFormat_Nellymoser = 6,
            SoundFormat_G711ALaw_PCM = 7,
            SoundFormat_G711muLaw_PCM = 8,
            SoundFormat_reserved = 9,
            SoundFormat_AAC = 10,
            SoundFormat_Speex = 11,
            SoundFormat_MP3_8KHz = 14,
            SoundFormat_DeviceSpecific_sound = 15
        };

    };
}
