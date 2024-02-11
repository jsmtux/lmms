import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
    required property string text
    property double value
    id: control
    Dial {
        implicitWidth: 30
        implicitHeight: 30
        inputMode: Dial.Horizontal
        value: control.value
    }
    Text {text: control.text}
}
