import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Theme
import App 1.0
import LmmsWidgets


Rectangle {
    required property BaseTrackModel track
    id: control
    signal instrumentActivated(InstrumentModel instrument)
    signal patternTrackActivated(PatternTrackModel patternTrack)

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
                value: control.track.volume.value
                inputMode: Dial.Horizontal
                onMoved: {
                    control.track.volume.value = volumeDial.value
                }
                from: control.track.volume.min
                to: control.track.volume.max
            }
            LmmsDial {
                model: control.track.volume
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
            checked: control.track.solo.value
            checkable: true
            down: control.track.solo.value
            onClicked: {
                control.track.solo.value = !control.track.solo.value
            }
            text: "S"
        }

        Button {
            width: 30
            height: 30
            checked: control.track.muted.value
            down: control.track.muted.value
            checkable: true
            onClicked: {
                control.track.muted.value = !control.track.muted.value
            }
            text: "M"
        }

        Button {
            height: 30
            width: 150
            text: control.track.name
            onClicked: {
                if (control.track.type == BaseTrackModel.Instrument) {
                    control.instrumentActivated(control.track.instrument)
                } else if (control.track.type == BaseTrackModel.Pattern) {
                    control.patternTrackActivated(control.track)
                }
            }
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