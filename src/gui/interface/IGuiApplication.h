#ifndef BDEA9425_99B2_40B9_9B74_68E9BF97708B
#define BDEA9425_99B2_40B9_9B74_68E9BF97708B

#include "IMainWindow.h"
#include "IMixerView.h"
#include "IPianoRollWindow.h"
#include "IAutomationEditor.h"

#include <QDomNode>

namespace lmms
{
class DataFile;
class Instrument;
class InstrumentTrack;

namespace gui
{

class IGuiApplication {
public:
    virtual ~IGuiApplication() {}
    virtual IMainWindow* mainWindowInterface() = 0;
	virtual IMixerView* mixerViewInterface() = 0;
	virtual IPianoRollWindow* pianoRollInterface() = 0;
	virtual IAutomationEditor* automationEditorInterface() = 0;

	virtual void clear() = 0;
	virtual void restoreState(QDomNode& node) = 0;
	virtual void saveState(DataFile& dataFile) = 0;

	virtual Instrument* createDummyInstrument(InstrumentTrack *_instrument_track) = 0;
};

// Short-hand function
LMMS_EXPORT IGuiApplication* getGUIInterface();

}
}

#endif /* BDEA9425_99B2_40B9_9B74_68E9BF97708B */