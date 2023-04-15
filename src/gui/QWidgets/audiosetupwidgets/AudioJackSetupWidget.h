#ifndef D1E123D0_EBCD_4377_AD26_42995CAD62C1
#define D1E123D0_EBCD_4377_AD26_42995CAD62C1

#include <QLineEdit>

#include "AudioDeviceSetupWidget.h"
#include "AudioJack.h"
#include "LcdSpinBox.h"

namespace lmms {

class AudioJackSetupWidget : public gui::AudioDeviceSetupWidget
{
public:
    AudioJackSetupWidget( QWidget * _parent );
    ~AudioJackSetupWidget() override;

    inline static QString name()
    {
        return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", AudioJack::name() );
    }

    void saveSettings() override;

private:
    QLineEdit * m_clientName;
    gui::LcdSpinBox * m_channels;

};

}

#endif /* D1E123D0_EBCD_4377_AD26_42995CAD62C1 */
