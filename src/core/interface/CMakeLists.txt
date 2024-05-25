ADD_LIBRARY(lmms_core_interface STATIC
    IAudioEngine.h
    IRenderManager.cpp
    IController.h
    IControllerConnection.h
    IConfigManager.h
    ValueBuffer.cpp
    ValueBuffer.h
    Model.cpp
    IModels.cpp
    JournallingObject.cpp
    SerializingObject.cpp
    IEffectChain.h
    IClip.h
    IEngine.h
    IEngine.cpp
    ITrack.h
    IPlugin.h
    IMidiPort.h
    IMidiClient.h
    IMidiClient.cpp
    IModels.h
    Clipboard.h
    AutomationNode.h
    AutomationNode.cpp
    Clipboard.cpp
    ISong.h
    TimePos.h
    TimePos.cpp
    Note.h
    Note.cpp
    IDetuningHelper.h
    IEnvelopeAndLfoParameters.h
    ITrackContainer.h
    ISampleBuffer.cpp)


find_package(Qt6 REQUIRED COMPONENTS Xml Gui) 

TARGET_LINK_LIBRARIES(lmms_core_interface
    PUBLIC
        Qt6::Xml
        Qt6::Gui
        ${SAMPLERATE_LIBRARIES}
        ${SNDFILE_LIBRARIES}
        ${FFTW3F_LIBRARIES}
    PUBLIC
        lmms_gui_interface
)

TARGET_INCLUDE_DIRECTORIES(lmms_core_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src/core ${SAMPLERATE_INCLUDE_DIRS} ${FFTW_INCLUDE_DIRS} )