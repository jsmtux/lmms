import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
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
        SpinBox {
        }
    }

    Component {
        id: instrumentTrackType
        Row {
            SpinBox {
                id: spinBoxControl
                value: control.track.instrumentProperties.volume
                onValueModified: {
                    control.track.instrumentProperties.volume = spinBoxControl.value
                }
            }
            SpinBox {
            }
        }
    }

    Component {
        id: automationTrackType
        Text { text: "" }
    }

    Component {
        id: patternTrackType
        Text { text: "" }
    }

    Row {
        spacing: 10
        padding: 5
        Button {
            width: 30
            height: 30
            checked: control.track.solo
            checkable: true
            down: control.track.solo
            onClicked: {
                control.track.solo = !control.track.solo
            }
            text: "S"
        }

        Button {
            width: 30
            height: 30
            checked: control.track.muted
            down: control.track.muted
            checkable: true
            onClicked: {
                control.track.muted = !control.track.muted
            }
            text: "M"
        }

        Button {
            height: 30
            width: 150
            text: control.track.name
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