#include "DummyInstrument.h"

namespace lmms {

IInstrument* InstantiateDummyInstrument(ITrack* _instrument_track)
{
    return new DummyInstrument(dynamic_cast<IInstrumentTrack*>(_instrument_track->getTrackTypeSpecificInterface()));
}

}