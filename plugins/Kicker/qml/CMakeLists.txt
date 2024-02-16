qt_add_library(kicker SHARED
    KickerGui.cpp
    KickerGui.h
    ../core/Kicker.h
    ../core/Kicker.cpp
    ../core/KickerOsc.h
)

set_property(TARGET kicker PROPERTY AUTOMOC ON)

qt_add_qml_module(kicker
    URI "kicker"
    VERSION 1.0
    QML_FILES
        BottomPanel.qml
    RESOURCE_PREFIX "/qt/qml/"
)

add_definitions(-DPLUGIN_NAME=kicker)

# INSTALL(TARGETS kicker
# LIBRARY DESTINATION "${PLUGIN_DIR}"
# RUNTIME DESTINATION "${PLUGIN_DIR}"
# )

INSTALL(TARGETS kickerplugin
LIBRARY DESTINATION "${PLUGIN_DIR}"
RUNTIME DESTINATION "${PLUGIN_DIR}"
)

IF(LMMS_BUILD_APPLE)
# IF ("${PLUGIN_LINK}" STREQUAL "SHARED")
    SET_TARGET_PROPERTIES(kicker PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
# ELSE()
#     SET_TARGET_PROPERTIES(kicker PROPERTIES LINK_FLAGS "-bundle_loader \"${CMAKE_BINARY_DIR}/lmms2example.app/Contents/MacOS/lmms2example\"")
# ENDIF()

ENDIF(LMMS_BUILD_APPLE)
IF(LMMS_BUILD_WIN32)
add_custom_command(
    TARGET "kickerplugin"
    POST_BUILD
    COMMAND "${STRIP_COMMAND}" "$<TARGET_FILE:kickerplugin>"
    VERBATIM
    COMMAND_EXPAND_LISTS
)
SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES PREFIX "")
ENDIF()

target_include_directories(kicker PUBLIC ../core)
target_link_libraries(kicker PUBLIC lmms_widgets lmms_core_plugins qmlgui)