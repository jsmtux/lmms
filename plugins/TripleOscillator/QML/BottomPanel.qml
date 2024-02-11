import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import App 1.0
import LmmsWidgets
import tripleoscillator

GroupBox {
    id: control
    required property TripleOscillatorModel instrument
    Layout.fillWidth: true
    Layout.minimumHeight: 350

    title: qsTr("TripleOscillator")

    ColumnLayout {
        anchors.fill: parent
        OscillatorPanel {
            text: "OSC1"
            osc: control.instrument.oscillators[0]
        }
        OscillatorPanel {
            text: "OSC2"
            osc: control.instrument.oscillators[1]
        }
        OscillatorPanel {
            text: "OSC3"
            osc: control.instrument.oscillators[2]
        }
    }
}