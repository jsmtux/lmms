import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LmmsWidgets
import audiofileprocessor
import App 1.0

GroupBox {
    Layout.fillWidth: true
    Layout.minimumHeight: 350
    required property AudioFileProcessorModel instrument

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