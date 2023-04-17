add_library(lmms_gui_interface INTERFACE)
target_include_directories(lmms_gui_interface INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/interface")

add_subdirectory(QWidgets)
