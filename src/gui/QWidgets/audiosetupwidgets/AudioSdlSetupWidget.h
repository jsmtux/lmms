#ifndef F5237355_DFED_4CDA_BBFF_AAB7240FB445
#define F5237355_DFED_4CDA_BBFF_AAB7240FB445

#include <QLineEdit>

#include "AudioDeviceSetupWidget.h"

namespace lmms {

class AudioSdlSetupWidget : public gui::AudioDeviceSetupWidget
{
public:
    AudioSdlSetupWidget( QWidget * _parent );
    ~AudioSdlSetupWidget() override = default;

    inline static QString name()
    {
        return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget",
                    "SDL (Simple DirectMedia Layer)" );
    }
    void saveSettings() override;

private:
    QLineEdit * m_device;

} ;

}

#endif /* F5237355_DFED_4CDA_BBFF_AAB7240FB445 */
