#ifndef LMMS_UI_AUDIO_SDL_SETUP_WIDGET_H
#define LMMS_UI_AUDIO_SDL_SETUP_WIDGET_H

#include <QLineEdit>

#include "AudioDeviceSetupWidget.h"
#include "lmmsconfig.h"
#include "AudioSdl.h"

#ifdef LMMS_HAVE_SDL

namespace lmms {

class AudioSdlSetupWidget : public gui::AudioDeviceSetupWidget
{
public:
    AudioSdlSetupWidget( QWidget * _parent );
    ~AudioSdlSetupWidget() override = default;

    inline static QString name() {
        return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", AudioSdl::name() );
    }

    void saveSettings() override;

private:
    QLineEdit * m_device;

} ;

}

#endif

#endif /* LMMS_UI_AUDIO_SDL_SETUP_WIDGET_H */
