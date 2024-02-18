ADD_SUBDIRECTORY(interface)
ADD_SUBDIRECTORY(plugin)

set(LMMS_SRCS
	AudioEngine.cpp
	AudioEngineProfiler.cpp
	AudioEngineWorkerThread.cpp
	AutomatableModel.cpp
	AutomationClip.cpp
	BandLimitedWave.cpp
	base64.cpp
	BufferManager.cpp
	ComboBoxModel.cpp
	ConfigManager.cpp
	Controller.cpp
	ControllerConnection.cpp
	CoreApplication.cpp
	DataFile.cpp
	DetuningHelper.cpp
	DrumSynth.cpp
	embed.cpp
	EffectChain.cpp
	Engine.cpp
	EnvelopeAndLfoParameters.cpp
	fft_helpers.cpp
	Mixer.cpp
	InlineAutomation.cpp
	InstrumentFunctions.cpp
	InstrumentPlayHandle.cpp
	InstrumentSoundShaping.cpp
	Keymap.cpp
	Ladspa2LMMS.cpp
	LadspaControl.cpp
	LadspaManager.cpp
	LfoController.cpp
	LocklessAllocator.cpp
	MemoryHelper.cpp
	MemoryManager.cpp
	MeterModel.cpp
	MicroTimer.cpp
	Microtuner.cpp
	MixHelpers.cpp
	ModelVisitor.cpp
	NotePlayHandle.cpp
	Oscillator.cpp
	PathUtil.cpp
	PatternClip.cpp
	PatternStore.cpp
	PeakController.cpp
	PerfLog.cpp
	Piano.cpp
	PlayHandle.cpp
	PluginIssue.cpp
	PresetPreviewPlayHandle.cpp
	ProjectJournal.cpp
	ProjectRenderer.cpp
	ProjectVersion.cpp
	RemotePlugin.cpp
	RemotePluginBase.cpp
	RenderManager.cpp
	RingBuffer.cpp
	SampleBuffer.cpp
	SampleClip.cpp
	SamplePlayHandle.cpp
	SampleRecordHandle.cpp
	Scale.cpp
	Song.cpp
	TempoSyncKnobModel.cpp
	Track.cpp
	TrackContainer.cpp
	Clip.cpp
	VstSyncController.cpp
	StepRecorder.cpp
	SharedMemory.cpp

	audio/AudioAlsa.cpp
	audio/AudioDevice.cpp
	audio/AudioDummy.cpp
	audio/AudioFileDevice.cpp
	audio/AudioFileMP3.cpp
	audio/AudioFileOgg.cpp
	audio/AudioFileFlac.cpp
	audio/AudioFileWave.cpp
	audio/AudioJack.cpp
	audio/AudioOss.cpp
	audio/AudioSndio.cpp
	audio/AudioPort.cpp
	audio/AudioPortAudio.cpp
	audio/AudioSoundIo.cpp
	audio/AudioPulseAudio.cpp
	audio/AudioSampleRecorder.cpp
	audio/AudioSdl.cpp

	lv2/Lv2Basics.cpp
	lv2/Lv2ControlBase.cpp
	lv2/Lv2Evbuf.cpp
	lv2/Lv2Features.cpp
	lv2/Lv2Ports.cpp
	lv2/Lv2Proc.cpp
	lv2/Lv2Manager.cpp
	lv2/Lv2Options.cpp
	lv2/Lv2SubPluginFeatures.cpp
	lv2/Lv2UridCache.cpp
	lv2/Lv2UridMap.cpp

	midi/MidiAlsaRaw.cpp
	midi/MidiAlsaSeq.cpp
	midi/MidiClient.cpp
	midi/MidiController.cpp
	midi/MidiEvent.cpp
	midi/MidiEventToByteSeq.cpp
	midi/MidiJack.cpp
	midi/MidiOss.cpp
	midi/MidiSndio.cpp
	midi/MidiApple.cpp
	midi/MidiPort.cpp
	midi/MidiWinMM.cpp

	tracks/AutomationTrack.cpp
	tracks/InstrumentTrack.cpp
	tracks/MidiClip.cpp
	tracks/PatternTrack.cpp
	tracks/SampleTrack.cpp
)

IF(LMMS_BUILD_WIN32)
	SET(EXTRA_LIBRARIES "winmm")
ENDIF()

IF(LMMS_BUILD_APPLE)
	SET(EXTRA_LIBRARIES "-framework CoreMIDI -framework CoreFoundation")
ENDIF()

if(LMMS_HAVE_OSS AND LMMS_BUILD_OPENBSD)
    SET(EXTRA_LIBRARIES "-lossaudio")
endif()

IF(LMMS_BUILD_HAIKU)
	SET(EXTRA_LIBRARIES "-lnetwork")
ENDIF()

if(LMMS_HAVE_LIBRT)
	list(APPEND EXTRA_LIBRARIES "rt")
endif()

if(LMMS_HAVE_PORTAUDIO)
	list(APPEND EXTRA_LIBRARIES portaudio)
endif()

if(LMMS_HAVE_MP3LAME)
	list(APPEND EXTRA_LIBRARIES mp3lame::mp3lame)
endif()

IF(LMMS_HAVE_WEAKJACK)
	LIST(APPEND LMMS_SRCS "${WEAKJACK_INCLUDE_DIRS}/weak_libjack.c")
ENDIF()

ADD_LIBRARY(lmms_core STATIC
	${LMMS_SRCS}
	${LMMS_INCLUDES}
	${LMMS_RCC_OUT}
)


FOREACH(LIB ${LMMS_REQUIRED_LIBS})
	IF(TARGET ${LIB})
		GET_TARGET_PROPERTY(INCLUDE_DIRS ${LIB} INTERFACE_INCLUDE_DIRECTORIES)
		IF(INCLUDE_DIRS)
			TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE ${INCLUDE_DIRS})
		ENDIF()
	ENDIF()
ENDFOREACH()

INCLUDE_DIRECTORIES(
	${JACK_INCLUDE_DIRS}
	${SAMPLERATE_INCLUDE_DIRS}
	${SNDFILE_INCLUDE_DIRS}
	${SNDIO_INCLUDE_DIRS}
	${FFTW3F_INCLUDE_DIRS}
)

IF(NOT LMMS_HAVE_SDL2 AND NOT ("${SDL_INCLUDE_DIR}" STREQUAL ""))
	TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE "${SDL_INCLUDE_DIR}")
ENDIF()

IF(LMMS_HAVE_WEAKJACK)
	LIST(APPEND LMMS_INCLUDES "${WEAKJACK_INCLUDE_DIRS}/weak_libjack.h")
	TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE "${WEAKJACK_INCLUDE_DIRS}")
	ADD_DEFINITIONS(-DUSE_WEAK_JACK=1 -DNO_JACK_METADATA=1)
ENDIF()

IF(NOT ("${PULSEAUDIO_INCLUDE_DIR}" STREQUAL ""))
	TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE "${PULSEAUDIO_INCLUDE_DIR}")
ENDIF()

IF(NOT ("${OGGVORBIS_INCLUDE_DIR}" STREQUAL ""))
	TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE "${OGGVORBIS_INCLUDE_DIR}")
ENDIF()

IF(NOT ("${LV2_INCLUDE_DIRS}" STREQUAL ""))
	TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE ${LV2_INCLUDE_DIRS})
ENDIF()

IF(NOT ("${LILV_INCLUDE_DIRS}" STREQUAL ""))
	TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE ${LILV_INCLUDE_DIRS})
ENDIF()

IF(NOT ("${SUIL_INCLUDE_DIRS}" STREQUAL ""))
	TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE ${SUIL_INCLUDE_DIRS})
ENDIF()
LIST(APPEND LMMS_SRCS "${RINGBUFFER_DIR}/src/lib/ringbuffer.cpp")

TARGET_INCLUDE_DIRECTORIES(lmms_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


SET(LMMS_REQUIRED_LIBS ${LMMS_REQUIRED_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
	${QT_LIBRARIES}
	${ASOUND_LIBRARY}
	${SDL_LIBRARY}
	${SDL2_LIBRARY}
	${SOUNDIO_LIBRARY}
	${SNDIO_LIBRARIES}
	${PULSEAUDIO_LIBRARIES}
	${JACK_LIBRARIES}
	${OGGVORBIS_LIBRARIES}
	${LV2_LIBRARIES}
	${SUIL_LIBRARIES}
	${LILV_LIBRARIES}
	${SAMPLERATE_LIBRARIES}
	${SNDFILE_LIBRARIES}
	${FFTW3F_LIBRARIES}
	${EXTRA_LIBRARIES}
	rpmalloc
)

# Expose required libs for tests binary
SET(LMMS_REQUIRED_LIBS ${LMMS_REQUIRED_LIBS} PARENT_SCOPE)

TARGET_LINK_LIBRARIES(lmms_core PUBLIC
	lmms_core_interface
	rpmalloc
	${LMMS_REQUIRED_LIBS}
	)

GENERATE_EXPORT_HEADER(lmms_core
	BASE_NAME lmms
)

# CMake doesn't define target_EXPORTS for OBJECT libraries.
# See the documentation of DEFINE_SYMBOL for details.
# Also add LMMS_STATIC_DEFINE for targets linking against it.
TARGET_COMPILE_DEFINITIONS(lmms_core
	PRIVATE -Dlmmsobjs_EXPORTS
	INTERFACE -DLMMS_STATIC_DEFINE
)

TARGET_INCLUDE_DIRECTORIES(lmms_core PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src ${CMAKE_CURRENT_BINARY_DIR})