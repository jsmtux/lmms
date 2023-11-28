#include "TripleOscillatorGui.h"

namespace lmms
{

extern "C"
{

PluginDescriptor kicker_plugin_descriptor =
{
	LMMS_STRINGIFY( PLUGIN_NAME ),
	TripleOscillator::PluginName(),
	QT_TRANSLATE_NOOP( "PluginBrowser",
				"Versatile drum synthesizer" ),
	"Tobias Doerffel <tobydox/at/users.sf.net>",
	0x0100,
	PluginTypes::Instrument,
	nullptr,
	nullptr,
	nullptr,
} ;

}

extern "C"
{

// necessary for getting instance out of shared lib
Plugin * lmms_plugin_main( void * m, void * )
{
	return new TripleOscillatorQmlInstrument( static_cast<IInstrumentTrack *>( m ) );
}


}

} //namespace lmms
