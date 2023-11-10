import QtQuick 2.0
import QtQuick.Layouts
import Theme
import App 1.0
import lmms2


Rectangle {
    required property BaseTrackModel track
    id: control

    height: 40
    width: 40
    anchors.left: parent.left
    anchors.right: parent.right
    gradient: Gradient {
        GradientStop {position: 0.0; color: "#485562"}
        GradientStop {position: 1.0; color: "#485562"}
    }
    border.color: "#485562"
    border.width: 2

    Component {
        id: sampleTrackType
        Row {
            spacing: 10
            Text { text: "SAMPLE" + control.track.name }
        }
    }

    Component {
        id: instrumentTrackType
        Row {
            spacing: 10
            Text { text: "INSTRUMENT" + control.track.name }
        }
    }

    Component {
        id: automationTrackType
        Row {
            spacing: 10
            Text { text: "AUTOMATION" + control.track.name }
        }
    }

    Component {
        id: patternTrackType
        Row {
            spacing: 10
            Text { text: "PATTERN" + control.track.name }
        }
    }

    Row {
        CheckBox {
            checked: control.track.solo
            onClicked: {
                control.track.solo = !control.track.solo
            }
            text: "Solo"
        }
        CheckBox {
            checked: control.track.muted
            onClicked: {
                control.track.muted = !control.track.muted
            }
            text: "Muted"
        }

        Loader {
            id: trackSpecificWidget
            sourceComponent: getComponentFromType(control.track.type)
        }
    }

    function getComponentFromType(type) {
        switch(type) {
            case BaseTrackModel.Sample:
                return sampleTrackType;
            case BaseTrackModel.Instrument:
                return instrumentTrackType;
            case BaseTrackModel.Automation:
                return automationTrackType;
            case BaseTrackModel.Pattern:
                return patternTrackType;
        }
    }
}