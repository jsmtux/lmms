#include "QWidgetDummyInstrument.h"

namespace lmms {

IInstrument* InstantiateDummyInstrument(IInstrumentTrack* _instrument_track)
{
    return new QWidgetDummyInstrument(_instrument_track);
}

}