#ifndef BDEA9425_99B2_40B9_9B74_68E9BF97708B
#define BDEA9425_99B2_40B9_9B74_68E9BF97708B

namespace lmms::gui
{

class MainWindow;
class MixerView;
class SongEditorWindow;
class PatternEditorWindow;
class PianoRollWindow;
class ProjectNotes;
class MicrotunerConfig;
class AutomationEditorWindow;
class ControllerRackView;

class IGuiApplication {
public:
    virtual ~IGuiApplication() {}
    virtual MainWindow* mainWindow() = 0;
	virtual MixerView* mixerView() = 0;
	virtual SongEditorWindow* songEditor() = 0;
	virtual PatternEditorWindow* patternEditor() = 0;
	virtual PianoRollWindow* pianoRoll() = 0;
	virtual ProjectNotes* getProjectNotes() = 0;
	virtual MicrotunerConfig* getMicrotunerConfig() = 0;
	virtual AutomationEditorWindow* automationEditor() = 0;
	virtual ControllerRackView* getControllerRackView() = 0;
};

// Short-hand function
LMMS_EXPORT IGuiApplication* getGUI();

}

#endif /* BDEA9425_99B2_40B9_9B74_68E9BF97708B */
