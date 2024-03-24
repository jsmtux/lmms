// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LmmsWidgets
import Qt.labs.folderlistmodel
import App 1.0

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
                Layout.minimumWidth: 600
                Layout.alignment: Qt.AlignHCenter
                color: "transparent"
                Rectangle {
                    id: statusBg
                    anchors.centerIn: parent
                    height: 100
                    width: parent.width
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
                            onClicked: songModel.Play();
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
            Layout.minimumWidth: 350
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

                    TreeView {
                        id: treeView
                        anchors.fill: parent
                        clip: true

                        delegate: TreeViewDelegate {}

                        model: lmms.projectFiles
                        rootIndex: lmms.projectsIndex
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

                    Component {
                        id: tabButton
                        TabButton {}
                    }

                    function addTab(patternTrack: PatternTrackModel) {
                        var btn = tabButton.createObject(mainAreaBar, {text: patternTrack.name})
                        mainAreaBar.addItem(btn)

                        var panel = patternTrackPanel.createObject(mainAreaGroup, {track: patternTrack})
                        mainAreaStack.children.push(panel)

                        mainAreaBar.currentIndex = 1
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

                    StackLayout {
                        width: parent.width
                        height: parent.height
                        id: mainAreaStack
                        currentIndex: mainAreaBar.currentIndex

                        RowLayout {
                            height: parent.height
                            VerticalHeaderView {
                                width: 350
                                syncView: songTableView
                                resizableRows: false
                                clip: true
                                model: songModel.trackList
                                delegate: TrackViewDelegate {
                                    Component.onCompleted: {
                                        instrumentActivated.connect(pluginAreaGroup.activateInstrument)
                                        patternTrackActivated.connect(mainAreaBar.addTab)
                                    }
                                }
                            }
                            TableView {
                                id: songTableView
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                clip: true
                                columnSpacing: 1
                                rowSpacing: 1

                                model:songModel.songTable

                                delegate: ClipViewDelegate {
                                    implicitHeight: 40
                                    implicitWidth: 80
                                }
                            }
                        }

                        Component {
                            id: patternTrackPanel
                            ColumnLayout {
                                id: control
                                required property PatternTrackModel track
                                height: parent.height
                                width: parent.width

                                Label {
                                    text: control.track.name
                                }

                                Component {
                                    id: patternTableDelegate
                                    CheckBox {
                                        id: control
                                        implicitWidth: 35
                                        required property PatternNoteModel note
                                        checked: control.note.enabled
                                        onClicked: control.note.enabled = checked
                                    }
                                }

                                RowLayout {
                                    VerticalHeaderView {
                                        width: 350
                                        Layout.fillHeight: true
                                        syncView: patternTableView
                                        resizableRows: false
                                        clip: true
                                        model: control.track.trackList
                                        delegate: TrackViewDelegate {
                                            Component.onCompleted: {
                                                instrumentActivated.connect(pluginAreaGroup.activateInstrument)
                                            }
                                        }
                                    }

                                    TableView {
                                        id: patternTableView
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                        clip: true
                                        columnSpacing: 1
                                        rowSpacing: 1

                                        model:control.track.patternTable

                                        delegate: patternTableDelegate
                                    }
                                }
                            }
                        }
                    }
                }

                GroupBox {
                    id: pluginAreaGroup
                    function activateInstrument(instrument: InstrumentModel) {
                        pluginUILoader.setSource("/qt/qml/" + instrument.name +  "/BottomPanel.qml", {"instrument": instrument})
                    }
                    Layout.fillWidth: true
                    Layout.minimumWidth: 250
                    Layout.minimumHeight: 250

                    Component {
                        id: emptyPluginComponent

                        Label {
                            text: "No instrument selected"
                        }
                    }

                    Loader {
                        id: pluginUILoader
                        width: parent.width
                        height: parent.height
                        sourceComponent: emptyPluginComponent
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
