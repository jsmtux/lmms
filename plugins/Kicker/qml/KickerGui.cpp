#include "KickerGui.h"

namespace lmms
{

extern "C"
{

// necessary for getting instance out of shared lib
Plugin * lmms_plugin_main( void * m, void * )
{
	return new KickerQmlInstrument( static_cast<IInstrumentTrack *>( m ) );
}


}

} //namespace lmms
