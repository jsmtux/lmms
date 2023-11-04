// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Templates as T
import Theme

T.ToolButton {
    id: control
    property string source;

    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                                         contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(background ? background.implicitHeight : 0,
                                          contentItem.implicitHeight + topPadding + bottomPadding)
    leftPadding: 4
    rightPadding: 4

    background: Rectangle {
        id: buttonBackground
        implicitWidth: 40
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.color: Theme.darkGray
        gradient: Gradient {
            GradientStop {position: 0.0; color: Theme.fgColor}
            GradientStop {position: 1.0; color: Theme.fgColorDarker}
        }
        border.width: 1
        radius: 2

        states: [
            State {
                name: "normal"
                when: !control.down
            },
            State {
                name: "down"
                when: control.down
                PropertyChanges {
                    buttonBackground.border.color: Theme.mainColorDarker
                }
            }
        ]
    }

    contentItem: Image {
        id: imageItem
        source: control.source

        opacity: 1.0
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        fillMode: Image.Pad

        states: [
            State {
                name: "normal"
                when: !control.down
            },
            State {
                name: "down"
                when: control.down
                PropertyChanges {
                    imageItem.opacity: 0.8
                }
            }
        ]
    }
}

