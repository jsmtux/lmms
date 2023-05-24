#pragma once

#include "Model.h"
#include "lmms_basics.h"
#include "TimePos.h"

namespace lmms
{

class IMidiEvent;
class IInstrumentTrack;

class IPiano {
public:
    virtual ~IPiano() = default;
    virtual Model* model() = 0;
    virtual void handleKeyPress(int key, int midiVelocity = -1) = 0;
    virtual void handleKeyRelease(int key) = 0;
    virtual bool isKeyPressed(int key) const = 0;
    virtual void processInEvent( const IMidiEvent* event, const TimePos& time = TimePos(), f_cnt_t offset = 0 ) = 0;
    virtual void setKeyState(int key, bool state) = 0;
	static bool isWhiteKey(int key);
	static bool isBlackKey(int key);
    virtual IInstrumentTrack* instrumentTrack() = 0;

	static const unsigned int WhiteKeysPerOctave = 7;
	static const unsigned int BlackKeysPerOctave = 5;
	static const unsigned int NumWhiteKeys = 75;
	static const unsigned int NumBlackKeys = 53;
};

} // namespace lmms