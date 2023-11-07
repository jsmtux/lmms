FIND_PACKAGE(Samplerate 0.1.8 MODULE REQUIRED)
ADD_LIBRARY(lmms_core_plugins STATIC Effect.cpp ExportFilter.cpp DummyEffect.h DummyInstrument ImportFilter.cpp Instrument.cpp ToolPlugin.cpp Plugin.cpp PluginFactory.cpp)

TARGET_INCLUDE_DIRECTORIES(lmms_core_plugins PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SAMPLERATE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(lmms_core_plugins lmms_core_interface lmms_gui_interface ${SAMPLERATE_LIBRARIES})