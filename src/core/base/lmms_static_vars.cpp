#include "lmms_static_vars.h"

namespace lmms {

const fpp_t MINIMUM_BUFFER_SIZE = 32;
const fpp_t DEFAULT_BUFFER_SIZE = 256;

fpp_t m_framesPerPeriod = DEFAULT_BUFFER_SIZE;

fpp_t framesPerPeriod()
{
    return m_framesPerPeriod;
}

bool setFramesPerPeriod(fpp_t frames) {
    bool ret = false;
    if(frames >= MINIMUM_BUFFER_SIZE) {
        m_framesPerPeriod = frames;
    }
    return ret;
}

}