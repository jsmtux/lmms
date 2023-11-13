# QT5_WRAP_CPP(kicker_MOC_out Kicker.h)
qt_generate_moc(Kicker.h Kicker.h.moc TARGET kicker_core)

qt_add_library(kicker_core STATIC
Kicker.cpp
Kicker.h
KickerOsc.h
Kicker.h.moc)


target_include_directories(kicker_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(kicker_core PUBLIC lmms_core_plugins lmms_gui_interface)
