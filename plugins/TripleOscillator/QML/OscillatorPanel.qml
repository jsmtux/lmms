import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import App 1.0
import LmmsWidgets

RowLayout {
    required property string text
    required property OscillatorObjectModel osc

    id: control

    Label {
        text: control.text
    }
    TaggedDial {
        text: "Vol"
        value: control.osc.volume
    }
    TaggedDial {
        text: "Pan"
        value: control.osc.pan
    }
    TaggedDial {
        text: "Crs"
        value: control.osc.coarse
    }
    TaggedDial {
        text: "Fl"
        value: control.osc.fineLeft
    }
    TaggedDial {
        text: "Fr"
        value: control.osc.fineRight
    }
    TaggedDial {
        text: "Pd"
        value: control.osc.phaseOffset
    }
    TaggedDial {
        text: "Spd"
        value: control.osc.stereoPhaseDetuning
    }
}