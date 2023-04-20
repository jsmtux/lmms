#ifndef BDEA9425_99B2_40B9_9B74_68E9BF97708B
#define BDEA9425_99B2_40B9_9B74_68E9BF97708B

#include "IMainWindow.h"
#include "IMixerView.h"
#include "ISongEditor.h"
#include "IPatternEditor.h"
#include "IPianoRollWindow.h"
#include "IProjectNotes.h"
#include "IAutomationEditor.h"

namespace lmms::gui
{

class ControllerRackView;

class IGuiApplication {
public:
    virtual ~IGuiApplication() {}
    virtual IMainWindow* mainWindowInterface() = 0;
	virtual IMixerView* mixerViewInterface() = 0;
	virtual ISongEditor* songEditorInterface() = 0;
	virtual IPatternEditor* patternEditorInterface() = 0;
	virtual IPianoRollWindow* pianoRollInterface() = 0;
	virtual IProjectNotes* getProjectNotesInterface() = 0;
	virtual IAutomationEditor* automationEditorInterface() = 0;
	virtual ControllerRackView* getControllerRackView() = 0;
};

// Short-hand function
LMMS_EXPORT IGuiApplication* getGUIInterface();

}

#endif /* BDEA9425_99B2_40B9_9B74_68E9BF97708B */
