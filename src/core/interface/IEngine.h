#pragma once

#include "IAudioEngine.h"
#include "ISong.h"

namespace lmms {

class IEngine
{
public:
    virtual ~IEngine() = default;
    virtual ISong* getSongInterface() = 0;
    virtual IAudioEngine* getAudioEngineInterface() = 0;
};

}