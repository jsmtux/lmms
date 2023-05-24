#ifndef LMMS_GUI_AUDIO_DUMMY_SETUP_WIDGET_H
#define LMMS_GUI_AUDIO_DUMMY_SETUP_WIDGET_H

#include "AudioDeviceSetupWidget.h"
#include "IAudioDevice.h"

namespace lmms {

class AudioDummySetupWidget : public gui::AudioDeviceSetupWidget
{
public:
    AudioDummySetupWidget( QWidget * _parent ) :
        gui::AudioDeviceSetupWidget( AudioDummySetupWidget::name(), _parent )
    {
    }

    inline static QString name()
    {
        return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", AudioDummyName() );
    }

    ~AudioDummySetupWidget() override = default;

    void saveSettings() override
    {
    }

    void show() override
    {
        parentWidget()->hide();
        QWidget::show();
    }

} ;

}

#endif /* LMMS_GUI_AUDIO_DUMMY_SETUP_WIDGET_H */
