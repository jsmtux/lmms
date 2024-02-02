// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Controls.Basic.impl
import QtQuick.Templates as T

T.Dial {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    background: DialImpl {
        implicitWidth: 184
        implicitHeight: 184
        color: control.visualFocus ? "#586471" : "#00dfe7"
        progress: control.position
        opacity: control.enabled ? 1 : 0.3
        startAngle: control.startAngle
        endAngle: control.endAngle
    }

    handle: Image {
        x: control.width / 2 - 10
        y: control.height / 2 - 10
        width: 20
        height: 20
        source: "qrc:/lmms2widgets/dial.png"
        antialiasing: true
        opacity: control.enabled ? 1 : 0.3
        transform: [
            Rotation {
                angle: control.angle
                origin.x: control.handle.width / 2
                origin.y: control.handle.height / 2
            }
        ]
    }
}
