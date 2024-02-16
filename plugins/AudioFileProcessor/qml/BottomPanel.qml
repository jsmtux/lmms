import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LmmsWidgets

GroupBox {
    Layout.fillWidth: true
    Layout.minimumHeight: 350

    title: qsTr("Audio File Processor")
    ColumnLayout {
        anchors.fill: parent
        ColumnLayout {
            TaggedDial {
                text: "Start"
            }
            TaggedDial {
                text: "End"
            }
        }
    }
}