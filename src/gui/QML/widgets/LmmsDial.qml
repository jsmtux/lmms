import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Theme
import App 1.0
import LmmsWidgets

ColumnLayout {
    required property FloatLmmsModel model
    property string text: ""
    id: control
    Dial {
        id: dial
        implicitWidth: 30
        implicitHeight: 30
        inputMode: Dial.Horizontal
        value: control.model.value
        from: control.model.min
        to: control.model.max
        onMoved: {
            control.model.value = dial.value
        }
    }
    Text {text: control.text}
}
