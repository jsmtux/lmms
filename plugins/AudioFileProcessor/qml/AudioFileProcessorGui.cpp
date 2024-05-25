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
	qmlRegisterType<AudioFileProcessorModel>("App", 1, 0, "AudioFileProcessorModel");
	return new AudioFileProcessorQmlInstrument( static_cast<IInstrumentTrack *>( m ) );
}


}

} //namespace gui
} //namespace lmms
