import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Theme
import App 1.0
import lmms2

Rectangle {
    required property ClipType clipType
    id: control

    Component {
        id: noneClipType
        Rectangle {
            color: "transparent"
        }
    }

    Component {
        id: instrumentClipType
        Rectangle {
            color: "orange"
        }
    }

    Component {
        id: patternClipType
        Rectangle {
            color: "green"
        }
    }

    Loader {
        sourceComponent: getComponentFromType(control.clipType)
        height: control.height
        width: control.width
    }

    function getComponentFromType(type) {
        switch(type) {
            case BaseClipModel.None:
                return noneClipType;
            case BaseClipModel.Instrument:
                return instrumentClipType;
            case BaseClipModel.Pattern:
                return patternClipType;
        }
    }
}