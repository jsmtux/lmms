#ifndef Q_WIDGET_INSTRUMENT_PLUGIN_H_
#define Q_WIDGET_INSTRUMENT_PLUGIN_H_

#include "Instrument.h"
#include "InstrumentTrack.h"
#include "Plugin.h"

namespace lmms::gui {

class QWidgetInstrumentPlugin : public Instrument {
public:
    QWidgetInstrumentPlugin(InstrumentTrack * _instrument_track,
                const Descriptor * _descriptor,
                const Descriptor::SubPluginFeatures::Key * key = nullptr) :
        Instrument(_instrument_track, _descriptor, key)
    {}

    //! Create a view for the model
	PluginView * createView( QWidget * parent ) {
        gui::PluginView * pv = instantiateView( parent );
        if( pv != nullptr )
        {
            pv->setModel( this );
        }
        return pv;
    }
protected:
	//! Create a view for the model
	virtual gui::PluginView* instantiateView( QWidget * ) = 0;
};

}

#endif // Q_WIDGET_INSTRUMENT_PLUGIN_H_