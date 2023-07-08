#ifndef LMMS_GUI_AUDIO_PULSE_AUDIO_SETUP_WIDGET_H
#define LMMS_GUI_AUDIO_PULSE_AUDIO_SETUP_WIDGET_H
<<<<<<< HEAD

#include "lmmsconfig.h"

#ifdef LMMS_HAVE_PULSEAUDIO
=======
>>>>>>> 680c4a277 (Fix some of the styling and DummyEffect implementation)

#include <QLineEdit>

#include "AudioDeviceSetupWidget.h"
#include "AudioPulseAudio.h"

#include "widgets/LcdSpinBox.h"

namespace lmms {

class AudioPulseAudioSetupWidget : public gui::AudioDeviceSetupWidget
{
public:
    AudioPulseAudioSetupWidget( QWidget * _parent );
    ~AudioPulseAudioSetupWidget() override;

inline static QString name()
{
    return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", AudioPulseAudio::name() );
}

    void saveSettings() override;

private:
    QLineEdit * m_device;
    gui::LcdSpinBox * m_channels;

} ;

}

#endif // LMMS_HAVE_PULSEAUDIO

#endif /* LMMS_GUI_AUDIO_PULSE_AUDIO_SETUP_WIDGET_H */
