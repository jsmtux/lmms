import QtQuick
import QtQuick.Layouts

ColumnLayout {
    required property string text
    id: control
    Dial {
        implicitWidth: 30
        implicitHeight: 30
        inputMode: Dial.Horizontal
    }
    Text {text: control.text}
}
