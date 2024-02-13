import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
    required property string text
    property double from: 0.0
    property double to: 1.0
    property double value
    id: control
    Dial {
        implicitWidth: 30
        implicitHeight: 30
        inputMode: Dial.Horizontal
        value: control.value
        from: control.from
        to: control.to
    }
    Text {text: control.text}
}
