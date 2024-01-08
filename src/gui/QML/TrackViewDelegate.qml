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
    implicitWidth: 350
    gradient: Gradient {
        GradientStop {position: 0.0; color: "#485562"}
        GradientStop {position: 1.0; color: "#485562"}
    }
    border.color: "#485562"
    border.width: 2

    Component {
        id: sampleTrackType
        Dial {
            implicitWidth: 30
            implicitHeight: 30
            inputMode: Dial.Horizontal
        }
    }

    Component {
        id: instrumentTrackType
        Row {
            spacing: 5
            Dial {
                id: volumeDial
                implicitWidth: 30
                implicitHeight: 30
                value: control.track.volume / 100.0
                inputMode: Dial.Horizontal
                onMoved: {
                    control.track.volume = volumeDial.value * 100
                }
            }
            Dial {
                implicitWidth: 30
                implicitHeight: 30
                inputMode: Dial.Horizontal
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