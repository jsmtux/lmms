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
        from: control.osc.volume_max_value
        to: control.osc.volume_min_value
    }
    TaggedDial {
        text: "Pan"
        value: control.osc.pan
        from: control.osc.pan_max_value
        to: control.osc.pan_min_value
    }
    TaggedDial {
        text: "Crs"
        value: control.osc.coarse
        from: control.osc.coarse_max_value
        to: control.osc.coarse_min_value
    }
    TaggedDial {
        text: "Fl"
        value: control.osc.fineLeft
        from: control.osc.fineLeft_max_value
        to: control.osc.fineLeft_min_value
    }
    TaggedDial {
        text: "Fr"
        value: control.osc.fineRight
        from: control.osc.fineRight_max_value
        to: control.osc.fineRight_min_value
    }
    TaggedDial {
        text: "Pd"
        value: control.osc.phaseOffset
        from: control.osc.phaseOffset_max_value
        to: control.osc.phaseOffset_min_value
    }
    TaggedDial {
        text: "Spd"
        value: control.osc.stereoPhaseDetuning
        from: control.osc.stereoPhaseDetuning_max_value
        to: control.osc.stereoPhaseDetuning_min_value
    }
}