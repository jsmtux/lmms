#ifndef LMMS_GUI_AUDIO_JACK_SETUP_WIDGET_H
#define LMMS_GUI_AUDIO_JACK_SETUP_WIDGET_H

#include <QLineEdit>

#include "IAudioDevice.h"
#include "AudioDeviceSetupWidget.h"

#include "widgets/LcdSpinBox.h"

namespace lmms {

class AudioJackSetupWidget : public gui::AudioDeviceSetupWidget
{
public:
    AudioJackSetupWidget( QWidget * _parent );
    ~AudioJackSetupWidget() override;

    inline static QString name()
    {
        return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", AudioJackName() );
    }

    void saveSettings() override;

private:
    QLineEdit * m_clientName;
    gui::LcdSpinBox * m_channels;

};

}

#endif /* LMMS_GUI_AUDIO_JACK_SETUP_WIDGET_H */
