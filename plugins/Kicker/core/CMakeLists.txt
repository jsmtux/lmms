QT5_WRAP_CPP(kicker_MOC_out Kicker.h)

add_library(kicker_core
    Kicker.cpp
    Kicker.h
    KickerOsc.h
    ${kicker_MOC_out})


target_include_directories(kicker_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(kicker_core PRIVATE lmms_core_plugins)