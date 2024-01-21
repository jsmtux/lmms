#ifndef LMMS_GUI_AUDIO_OSS_SETUP_WIDGET_H
#define LMMS_GUI_AUDIO_OSS_SETUP_WIDGET_H

#include "lmmsconfig.h"

#ifdef LMMS_HAVE_OSS

#include "AudioOss.h"
#include "AudioDeviceSetupWidget.h"

#include "widgets/LcdSpinBox.h"

#include <QWidget>
#include <QLineEdit>

namespace lmms {

class AudioOssSetupWidget : public gui::AudioDeviceSetupWidget
{
public:
    AudioOssSetupWidget( QWidget * _parent );
    ~AudioOssSetupWidget() override;

inline static QString name()
{
    return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", AudioOss::name() );
}

    void saveSettings() override;

private:
    QLineEdit * m_device;
    gui::LcdSpinBox * m_channels;

} ;

}

#endif // LMMS_HAVE_OSS

#endif /* LMMS_GUI_AUDIO_OSS_SETUP_WIDGET_H */
