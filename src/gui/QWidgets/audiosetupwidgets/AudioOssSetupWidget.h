#ifndef D98C1538_0DF8_4987_8C01_08FF2A459AB3
#define D98C1538_0DF8_4987_8C01_08FF2A459AB3

#include <QWidget>
#include <QLineEdit>

#include "AudioOss.h"
#include "AudioDeviceSetupWidget.h"

#include "widgets/LcdSpinBox.h"

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

#endif /* D98C1538_0DF8_4987_8C01_08FF2A459AB3 */
