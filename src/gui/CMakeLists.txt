add_library(lmms_gui_interface STATIC interface/ITimeLineWidget.cpp)
find_package(Qt6 REQUIRED COMPONENTS Xml) 

TARGET_LINK_LIBRARIES(lmms_gui_interface
    PRIVATE
        Qt6::Xml
)
target_include_directories(lmms_gui_interface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/interface")

# add_subdirectory(QWidgets)
add_subdirectory(headless)
add_subdirectory(QML)
