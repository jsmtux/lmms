#pragma once

#include <QString>

#include "lmms_basics.h"

namespace lmms
{

inline QString AudioJackName() {
    return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget",
        "JACK (JACK Audio Connection Kit)" );
}

inline QString PortAudioName()
{
    return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", "PortAudio" );
}

inline QString AudioSdlName()
{
    return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget",
					"SDL (Simple DirectMedia Layer)" );
}

inline QString AudioDummyName()
{
    return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", "Dummy (no sound output)" );
}

inline QString MidiJackName()
{
    return( QT_TRANSLATE_NOOP( "MidiSetupWidget",
			"Jack-MIDI" ) );
}

class IAudioDevice
{
public:
    virtual ch_cnt_t channels() const = 0;
    virtual bool supportsCapture() const = 0;
};

} // namespace lmms
