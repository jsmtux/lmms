import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Theme

T.TabButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    spacing: 6

    icon.width: 24
    icon.height: 24
    icon.color: checked ? control.palette.windowText : control.palette.brightText

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.checked ? control.palette.windowText : control.palette.brightText
    }

    background: Rectangle {
        implicitHeight: 40

        gradient: Gradient {
            GradientStop {position: 0.0; color: control.checked ? Theme.fgColor : Theme.tabBgColor}
            GradientStop {position: 1.0; color: control.checked ? Theme.fgColorDarker : Theme.tabBgColor}
        }
        border.color : Theme.tabBgColorDarker
    }
}
