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
	qmlRegisterType<gui::TripleOscillatorModel>("App", 1, 0, "TripleOscillatorModel");
	qmlRegisterType<gui::OscillatorObjectModel>("App", 1, 0, "OscillatorObjectModel");
	return new gui::TripleOscillatorQmlInstrument( static_cast<IInstrumentTrack *>( m ) );
}


}

} //namespace lmms
