#include "QWidgetDummyInstrument.h"

namespace lmms {

IInstrument* InstantiateDummyInstrument(ITrack* _instrument_track)
{
    return new QWidgetDummyInstrument(dynamic_cast<IInstrumentTrack*>(_instrument_track->getTrackTypeSpecificInterface()));
}

}