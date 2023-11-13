// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Theme

pragma ComponentBehavior: Bound

ApplicationWindow {
    id: appWindow

    visible: true
    title: qsTr("Basic layouts")
    readonly property int margin: 11

    Component.onCompleted: {
        width = mainLayout.implicitWidth + 2 * margin
        height = mainLayout.implicitHeight + 2 * margin
    }

    color: Theme.fgColor
    minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
    minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0

        RowLayout {
            id: menuBar
            Layout.fillWidth: true
            Layout.margins: 5
            
            RowLayout {
                Layout.margins: 5
                id: menuBarLeftButtons
                HeaderButton {
                    id: saveButton
                    source: "qrc:/icons/big_button/save.png"
                    focusPolicy: Qt.TabFocus
                }
                HeaderButton {
                    id: helpButton
                    source: "qrc:/icons/big_button/help.png"
                    focusPolicy: Qt.TabFocus
                }
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.minimumHeight: 100
                Layout.minimumWidth: 400
                Layout.alignment: Qt.AlignHCenter
                color: "transparent"
                Rectangle {
                    id: statusBg
                    anchors.centerIn: parent
                    height: 100
                    width: 400
                    color: Theme.bgColor
                    border.color: Theme.bgColorDarker
                    border.width: 2

                    Flow {
                        anchors.verticalCenter: statusBg.verticalCenter
                        padding: 5
                        StatusButton {
                            id: helpButtonTest
                            source: "qrc:/icons/status_button/play.png"
                            focusPolicy: Qt.TabFocus
                        }
                        StatusButton {
                            id: helpButtonTest3
                            source: "qrc:/icons/status_button/record.png"
                            focusPolicy: Qt.TabFocus
                        }
                        StatusButton {
                            id: helpButtonTest4
                            source: "qrc:/icons/status_button/play_record.png"
                            focusPolicy: Qt.TabFocus
                        }
                        StatusButton {
                            id: helpButtonTest5
                            source: "qrc:/icons/status_button/stop.png"
                            focusPolicy: Qt.TabFocus
                        }
                    }
                }
            }
        }
        
        RowLayout {
            id: workArea
            Layout.fillHeight: true
            Layout.minimumWidth: 250
            spacing: 0

            ColumnLayout {
                Layout.fillHeight: true
                spacing: 0

                TabBar {
                    id: projectBar
                    width: parent.width
                    TabButton {
                        text: qsTr("Browser")
                    }
                }

                GroupBox {
                    id: rowBox
                    Layout.fillHeight: true
                    Layout.minimumWidth: 250

                    background: Rectangle {
                        width: parent.width
                        height: parent.height
                        color: Theme.bgColor
                        border.color: Theme.tabBgColorDarker
                        border.width: 5
                    }
                }
            }


            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 600
                Layout.minimumHeight: 800
                spacing: 0

                TabBar {
                    id: mainAreaBar
                    z: 10
                    width: parent.width
                    TabButton {
                        text: qsTr("Song Editor")
                    }
                    TabButton {
                        text: qsTr("BB Editor")
                    }
                }


                GroupBox {
                    id: mainAreaGroup
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumWidth: 250
                    padding: 3

                    background: Rectangle {
                        width: parent.width
                        height: parent.height
                        color: Theme.bgColor
                        border.color: Theme.tabBgColorDarker
                        border.width: 2
                    }

                    Row {
                        anchors.fill: parent
                        height: parent.height
                        ListView {
                            width: 350
                            height: parent.height
                            model: trackModel
                            spacing: 1
                            delegate: TrackViewDelegate {}
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillHeight: true
                spacing: 0

                TabBar {
                    width: parent.width
                    TabButton {
                        text: qsTr("Controller")
                    }
                    TabButton {
                        text: qsTr("Notes")
                    }
                }


                GroupBox {
                    Layout.fillHeight: true
                    Layout.minimumWidth: 250

                    background: Rectangle {
                        width: parent.width
                        height: parent.height
                        color: Theme.bgColor
                        border.color: Theme.tabBgColorDarker
                        border.width: 5
                    }
                }
            }
        }
    }
}
