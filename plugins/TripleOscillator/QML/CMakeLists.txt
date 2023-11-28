qt_add_library(tripleoscillator MODULE
    TripleOscillatorGui.cpp
    TripleOscillatorGui.h
    ../core/TripleOscillator.cpp
    ../core/TripleOscillator.h
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")

set_property(TARGET tripleoscillator PROPERTY AUTOMOC ON)

add_definitions(-DPLUGIN_NAME=tripleoscillator)

INSTALL(TARGETS tripleoscillator
LIBRARY DESTINATION "${PLUGIN_DIR}"
RUNTIME DESTINATION "${PLUGIN_DIR}"
)

IF(LMMS_BUILD_APPLE)
IF ("${PLUGIN_LINK}" STREQUAL "SHARED")
    SET_TARGET_PROPERTIES(tripleoscillator PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
ELSE()
    SET_TARGET_PROPERTIES(tripleoscillator PROPERTIES LINK_FLAGS "-bundle_loader \"${CMAKE_BINARY_DIR}/lmms2example.app/Contents/MacOS/lmms2example\"")
ENDIF()

ENDIF(LMMS_BUILD_APPLE)
IF(LMMS_BUILD_WIN32)
add_custom_command(
    TARGET "tripleoscillator"
    POST_BUILD
    COMMAND "${STRIP_COMMAND}" "$<TARGET_FILE:tripleoscillator>"
    VERBATIM
    COMMAND_EXPAND_LISTS
)
SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES PREFIX "")
ENDIF()

target_include_directories(tripleoscillator PUBLIC ../core)
target_link_libraries(tripleoscillator PUBLIC lmms_core_interface lmmsobjs qmlgui)