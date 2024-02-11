import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import lmms2widgets

GroupBox {
    Layout.fillWidth: true
    Layout.minimumHeight: 350

    title: qsTr("Plugin information")
    ColumnLayout {
        anchors.fill: parent
        ColumnLayout {
        Dial {
            implicitWidth: 30
            implicitHeight: 30
        }
        Text {text: "Start"}
        }
        ColumnLayout {
        Dial {
            implicitWidth: 30
            implicitHeight: 30
        }
        Text {text: "End"}
        }
    }
}