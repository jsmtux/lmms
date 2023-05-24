#include "MidiEvent.h"

namespace lmms
{

std::unique_ptr<IMidiEvent> createMidiEvent(MidiEventTypes type,
				int8_t channel,
				int16_t param1,
				int16_t param2,
				const void* sourcePort,
				IMidiEvent::Source source)
{
    return std::make_unique<MidiEvent>(type,
                        channel,
                        param1,
                        param2,
                        sourcePort,
                        source);
}

}