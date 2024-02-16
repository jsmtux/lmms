#include "ISampleBuffer.h"
#include "IEngine.h"

namespace lmms {

std::unique_ptr<ISampleBuffer> createSampleBuffer() {
    return IEngine::Instance()->createSampleBuffer();
}
IHandleState* createHandleState(bool varyingPitch, int interpolationMode) {
    return IEngine::Instance()->createHandleState(varyingPitch, interpolationMode);
}

} // namespace lmms