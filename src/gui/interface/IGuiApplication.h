#ifndef BDEA9425_99B2_40B9_9B74_68E9BF97708B
#define BDEA9425_99B2_40B9_9B74_68E9BF97708B

#include "IMainWindow.h"
#include "IMixerView.h"
#include "ISongEditor.h"
#include "IPatternEditor.h"
#include "IPianoRollWindow.h"

namespace lmms::gui
{

class SongEditorWindow;
class ProjectNotes;
class MicrotunerConfig;
class AutomationEditorWindow;
class ControllerRackView;

class IGuiApplication {
public:
    virtual ~IGuiApplication() {}
    virtual IMainWindow* mainWindowInterface() = 0;
	virtual IMixerView* mixerViewInterface() = 0;
	virtual ISongEditor* songEditorInterface() = 0;
	virtual IPatternEditor* patternEditorInterface() = 0;
	virtual IPianoRollWindow* pianoRollInterface() = 0;
	virtual ProjectNotes* getProjectNotes() = 0;
	virtual MicrotunerConfig* getMicrotunerConfig() = 0;
	virtual AutomationEditorWindow* automationEditor() = 0;
	virtual ControllerRackView* getControllerRackView() = 0;
};

// Short-hand function
LMMS_EXPORT IGuiApplication* getGUIInterface();

}

#endif /* BDEA9425_99B2_40B9_9B74_68E9BF97708B */
