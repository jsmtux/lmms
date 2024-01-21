ADD_LIBRARY(lmms_core_interface STATIC
    IAudioEngine.h
    IRenderManager.cpp
    IController.h
    IControllerConnection.h
    IConfigManager.h
    ValueBuffer.cpp
    ValueBuffer.h
    Model.cpp
    JournallingObject.cpp
    SerializingObject.cpp
    IEffectChain.h
    IClip.h
    IEngine.h
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
    Note.h
    Note.cpp
    IDetuningHelper.h
    IEnvelopeAndLfoParameters.h
    ITrackContainer.h)

TARGET_INCLUDE_DIRECTORIES(lmms_core_interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src/core)