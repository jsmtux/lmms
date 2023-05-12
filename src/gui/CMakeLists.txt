add_library(lmms_gui_interface STATIC interface/ITimeLineWidget.cpp)
target_include_directories(lmms_gui_interface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/interface")

add_subdirectory(QWidgets)
add_subdirectory(headless)
