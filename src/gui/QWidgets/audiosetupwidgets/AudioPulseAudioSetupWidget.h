#ifndef D693ED99_F5AA_49A5_921F_D1AE0E3D46CC
#define D693ED99_F5AA_49A5_921F_D1AE0E3D46CC

#include <QLineEdit>

#include "AudioDeviceSetupWidget.h"
#include "AudioPulseAudio.h"
#include "LcdSpinBox.h"

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

#endif /* D693ED99_F5AA_49A5_921F_D1AE0E3D46CC */
