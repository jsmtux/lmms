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
    LmmsDial {
        text: "Vol"
        model: control.osc.volume
    }
    LmmsDial {
        text: "Pan"
        model: control.osc.pan
    }
    LmmsDial {
        text: "Crs"
        model: control.osc.coarse
    }
    LmmsDial {
        text: "Fl"
        model: control.osc.fineLeft
    }
    LmmsDial {
        text: "Fr"
        model: control.osc.fineRight
    }
    LmmsDial {
        text: "Pd"
        model: control.osc.phaseOffset
    }
    LmmsDial {
        text: "Spd"
        model: control.osc.stereoPhaseDetuning
    }
}