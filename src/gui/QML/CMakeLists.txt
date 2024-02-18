# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

project(lmms2 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/quickcontrols/lmms2")

qt_standard_project_setup(REQUIRES 6.5)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Widgets)

qt_add_library(qmlgui SHARED
    GuiApplication.cpp
    GuiApplication.h
    LMMSProperty.h
)

target_include_directories(qmlgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(qmlgui PUBLIC 
    lmms_core
    lmms_core_plugins
)

add_subdirectory(widgets)

target_link_libraries(qmlgui PUBLIC 
    Qt6::Qml
    lmms_core_interface
    lmms_core_plugins
    lmms_widgetsplugin
)

qt_add_executable(lmms2example WIN32 MACOSX_BUNDLE
    main.cpp
)

qt_add_qml_module(lmms2example
    URI lmms2
    NO_RESOURCE_TARGET_PATH
    QML_FILES
        "lmms2.qml"
        "TrackViewDelegate.qml"
        "ClipViewDelegate.qml"
        "imports/Flat/Button.qml"
        "imports/Flat/Dial.qml"
        "imports/Flat/HeaderButton.qml"
        "imports/Flat/StatusButton.qml"
        "imports/Flat/CheckBox.qml"
        "imports/Flat/Switch.qml"
        "imports/Flat/TabBar.qml"
        "imports/Flat/TabButton.qml"
        "imports/Theme/Theme.qml"
    RESOURCES
        "imports/Flat/qmldir"
        "imports/Theme/qmldir"
        "icons/controls/dial.png"
        "icons/status_button/stop.png"
        "icons/status_button/play_record.png"
        "icons/status_button/record.png"
        "icons/status_button/play.png"
        "icons/big_button/help.png"
        "icons/big_button/save.png"
        "icons/small_button/m.png"
        "icons/small_button/s.png"
        "icons/small_button/gear.png"
        "icons/small_button/patterntrack.png"
        "qtquickcontrols2.conf"
)

target_link_libraries(lmms2example PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Qml
    Qt6::Widgets
    lmms_core
    qmlgui
)

install(TARGETS lmms2example
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
