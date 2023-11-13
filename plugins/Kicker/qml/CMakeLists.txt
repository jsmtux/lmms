qt_add_library(kicker MODULE
    KickerGui.cpp
    KickerGui.h
    ../core/Kicker.h
    ../core/Kicker.cpp
    ../core/KickerOsc.h
)

set_property(TARGET kicker PROPERTY AUTOMOC ON)

add_definitions(-DPLUGIN_NAME=kicker)

INSTALL(TARGETS kicker
LIBRARY DESTINATION "${PLUGIN_DIR}"
RUNTIME DESTINATION "${PLUGIN_DIR}"
)

IF(LMMS_BUILD_APPLE)
IF ("${PLUGIN_LINK}" STREQUAL "SHARED")
    SET_TARGET_PROPERTIES(kicker PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
ELSE()
    SET_TARGET_PROPERTIES(kicker PROPERTIES LINK_FLAGS "-bundle_loader \"${CMAKE_BINARY_DIR}/lmms2example.app/Contents/MacOS/lmms2example\"")
ENDIF()

ENDIF(LMMS_BUILD_APPLE)
IF(LMMS_BUILD_WIN32)
add_custom_command(
    TARGET "kicker"
    POST_BUILD
    COMMAND "${STRIP_COMMAND}" "$<TARGET_FILE:kicker>"
    VERBATIM
    COMMAND_EXPAND_LISTS
)
SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES PREFIX "")
ENDIF()

target_include_directories(kicker PUBLIC ../core)
target_link_libraries(kicker PUBLIC lmms_core_interface lmmsobjs qmlgui)