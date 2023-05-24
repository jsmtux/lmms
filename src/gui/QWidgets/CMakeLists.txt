SET(LMMS_UI_SRCS
	ActionGroup.cpp
	AudioDeviceSetupWidget.cpp
	AutomatableModelView.cpp
	ControlLayout.cpp
	ControllerRackView.cpp
	ControllerView.cpp
	Controls.cpp
	EffectRackView.cpp
	EffectView.cpp
	FileBrowser.cpp
	GuiApplication.cpp
	# LadspaControlView.cpp
	LinkedModelGroupViews.cpp
	LmmsPalette.cpp
	LmmsStyle.cpp
	Lv2ViewBase.cpp
	MainApplication.cpp
	MainWindow.cpp
	MicrotunerConfig.cpp
	MidiCCRackView.cpp
	MidiSetupWidget.cpp
	MixerLine.cpp
	MixerView.cpp
	PluginBrowser.cpp
	ProjectNotes.cpp
	RowTableView.cpp
	SampleTrackWindow.cpp
	SendButtonIndicator.cpp
	SideBar.cpp
	SideBarWidget.cpp
	StringPairDrag.cpp
	SubWindow.cpp
	ToolPluginView.cpp
	TabButton.cpp

	audiosetupwidgets/AudioAlsaSetupWidget.cpp
	audiosetupwidgets/AudioJackSetupWidget.cpp
	audiosetupwidgets/AudioPortAudioSetupWidget.cpp
	audiosetupwidgets/AudioPulseAudioSetupWidget.cpp
	audiosetupwidgets/AudioSdlSetupWidget.cpp
	audiosetupwidgets/AudioOssSetupWidget.cpp

	clips/AutomationClipView.cpp
	clips/ClipView.cpp
	clips/MidiClipView.cpp
	clips/PatternClipView.cpp
	clips/SampleClipView.cpp
	clips/ClipViewFactory.cpp

	controllerdialogs/ControllerDialogsFactory.cpp
	controllerdialogs/ControllerDialog.cpp
	controllerdialogs/PeakControllerDialog.cpp
	controllerdialogs/LfoControllerDialog.cpp

	editors/AutomationEditor.cpp
	editors/Editor.cpp
	editors/PatternEditor.cpp
	editors/PianoRoll.cpp
	editors/PositionLine.cpp
	editors/Rubberband.cpp
	editors/SongEditor.cpp
	editors/StepRecorderWidget.cpp
	editors/TimeLineWidget.cpp
	editors/TrackContainerView.cpp

	effectcontroldialogs/EffectControlDialog.cpp

	instrument/EnvelopeAndLfoView.cpp
	instrument/InstrumentFunctionViews.cpp
	instrument/InstrumentMidiIOView.cpp
	instrument/InstrumentMiscView.cpp
	instrument/InstrumentSoundShapingView.cpp
	instrument/InstrumentTrackWindow.cpp
	instrument/InstrumentView.cpp
	instrument/PianoView.cpp

	menus/MidiPortMenu.cpp
	menus/RecentProjectsMenu.cpp
	menus/TemplatesMenu.cpp

	modals/AboutDialog.cpp
	modals/ColorChooser.cpp
	modals/ControllerConnectionDialog.cpp
	modals/EffectSelectDialog.cpp
	modals/ExportProjectDialog.cpp
	modals/FileDialog.cpp
	modals/RenameDialog.cpp
	modals/SetupDialog.cpp
	modals/VersionedSaveDialog.cpp

	# plugins/DummyEffect.cpp

	tracks/AutomationTrackView.cpp
	tracks/FadeButton.cpp
	tracks/InstrumentTrackView.cpp
	tracks/PatternTrackView.cpp
	tracks/SampleTrackView.cpp
	tracks/TrackContentWidget.cpp
	tracks/TrackLabelButton.cpp
	tracks/TrackOperationsWidget.cpp
	tracks/TrackRenameLineEdit.cpp
	tracks/TrackView.cpp
	tracks/TrackViewFactory.cpp

	widgets/AutomatableButton.cpp
	widgets/AutomatableSlider.cpp
	widgets/CPULoadWidget.cpp
	widgets/CaptionMenu.cpp
	widgets/ComboBox.cpp
	widgets/CustomTextKnob.cpp
	widgets/Fader.cpp
	widgets/Graph.cpp
	widgets/GroupBox.cpp
	widgets/Knob.cpp
	widgets/LcdFloatSpinBox.cpp
	widgets/LcdSpinBox.cpp
	widgets/LcdWidget.cpp
	widgets/LedCheckBox.cpp
	widgets/LeftRightNav.cpp
	widgets/MeterDialog.cpp
	widgets/MixerLineLcdSpinBox.cpp
	widgets/NStateButton.cpp
	widgets/Oscilloscope.cpp
	widgets/PixmapButton.cpp
	widgets/SimpleTextFloat.cpp
	widgets/TabBar.cpp
	widgets/TabWidget.cpp
	widgets/TempoSyncKnob.cpp
	widgets/TextFloat.cpp
	widgets/TimeDisplayWidget.cpp
	widgets/ToolButton.cpp
)

set(LMMS_UIS
	modals/about_dialog.ui
	modals/EffectSelectDialog.ui
	modals/export_project.ui
)

QT5_WRAP_UI(LMMS_UI_OUT ${LMMS_UIS})
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${LMMS_UI_OUT})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

FIND_PACKAGE(Samplerate 0.1.8 MODULE REQUIRED)

add_library(lmms_gui STATIC ${LMMS_UI_SRCS} ${LMMS_UI_OUT})
target_include_directories(lmms_gui PUBLIC ${SAMPLERATE_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/include" "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/src" "${CMAKE_BINARY_DIR}/src/core" "${WEAKJACK_INCLUDE_DIRS}")
target_link_libraries(lmms_gui PUBLIC lmmsobjs PRIVATE Qt5::Widgets)

GENERATE_EXPORT_HEADER(lmms_gui
	BASE_NAME lmms
)
