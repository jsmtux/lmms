#ifndef Q_WIDGET_INSTRUMENT_PLUGIN_H_
#define Q_WIDGET_INSTRUMENT_PLUGIN_H_

#include "Instrument.h"
#include "ITrack.h"
#include "instrument/InstrumentView.h"
#include "interface/IQWidgetInstrumentPlugin.h"
#include "Plugin.h"

namespace lmms::gui {

class QWidgetInstrumentPlugin : public Instrument, public IQWidgetInstrumentPlugin {
public:
    QWidgetInstrumentPlugin(IInstrumentTrack * _instrument_track,
                const PluginDescriptor * _descriptor,
                const PluginDescriptor::Key * key = nullptr) :
        Instrument(_instrument_track->baseTrack(), _descriptor, key)
    {}

    //! Create a view for the model
	PluginView * createView( QWidget * parent ) override {
        gui::InstrumentView * pv = instantiateView( parent );
        return pv;
    }
protected:
	//! Create a view for the model
	virtual gui::InstrumentView* instantiateView( QWidget * ) = 0;
};

}

#endif // Q_WIDGET_INSTRUMENT_PLUGIN_H_