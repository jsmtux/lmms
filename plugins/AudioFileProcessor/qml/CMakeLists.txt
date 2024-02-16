qt_add_library(audiofileprocessor SHARED
    AudioFileProcessorGui.cpp
    AudioFileProcessorGui.h
    ../core/AudioFileProcessor.h
    ../core/AudioFileProcessor.cpp
)

set_property(TARGET audiofileprocessor PROPERTY AUTOMOC ON)

qt_add_qml_module(audiofileprocessor
    URI audiofileprocessor
    VERSION 1.0
    QML_FILES
        BottomPanel.qml
    RESOURCE_PREFIX "/qt/qml/"
)

add_definitions(-DPLUGIN_NAME=audiofileprocessor)

# INSTALL(TARGETS audiofileprocessor
# LIBRARY DESTINATION "${PLUGIN_DIR}"
# RUNTIME DESTINATION "${PLUGIN_DIR}"
# )

INSTALL(TARGETS audiofileprocessorplugin
LIBRARY DESTINATION "${PLUGIN_DIR}"
RUNTIME DESTINATION "${PLUGIN_DIR}"
)

IF(LMMS_BUILD_APPLE)
# IF ("${PLUGIN_LINK}" STREQUAL "SHARED")
    SET_TARGET_PROPERTIES(audiofileprocessor PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
# ELSE()
#     SET_TARGET_PROPERTIES(audiofileprocessor PROPERTIES LINK_FLAGS "-bundle_loader \"${CMAKE_BINARY_DIR}/lmms2example.app/Contents/MacOS/lmms2example\"")
# ENDIF()
ENDIF(LMMS_BUILD_APPLE)

IF(LMMS_BUILD_WIN32)
add_custom_command(
    TARGET "audiofileprocessorplugin"
    POST_BUILD
    COMMAND "${STRIP_COMMAND}" "$<TARGET_FILE:audiofileprocessorplugin>"
    VERBATIM
    COMMAND_EXPAND_LISTS
)
SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES PREFIX "")
ENDIF()

target_include_directories(audiofileprocessor PUBLIC ../core)
target_link_libraries(audiofileprocessor PUBLIC lmms_widgets lmms_core_plugins qmlgui)