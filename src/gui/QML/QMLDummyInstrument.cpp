#include "DummyEffect.h"
#include "Instrument.h"
#include "DummyInstrument.h"

namespace lmms
{

IEffect* InstantiateDummyEffect(IEffectChain* _parent, const QDomElement& originalPluginData)
{
	return new DummyEffect(_parent, originalPluginData);
}


IInstrument* InstantiateDummyInstrument(IInstrumentTrack* _instrument_track)
{
	return new DummyInstrument(_instrument_track);
}
}