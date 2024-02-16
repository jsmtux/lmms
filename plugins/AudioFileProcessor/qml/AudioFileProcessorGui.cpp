#include "AudioFileProcessorGui.h"

namespace lmms
{
namespace gui
{

extern "C"
{

// necessary for getting instance out of shared lib
Plugin * lmms_plugin_main( void * m, void * )
{
	// qmlRegisterType<KickerModel>("App", 1, 0, "KickerModel");
	return new AudioFileProcessorQmlInstrument( static_cast<IInstrumentTrack *>( m ) );
}


}

} //namespace gui
} //namespace lmms
