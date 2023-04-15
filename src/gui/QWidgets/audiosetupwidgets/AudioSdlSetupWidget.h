#ifndef F5237355_DFED_4CDA_BBFF_AAB7240FB445
#define F5237355_DFED_4CDA_BBFF_AAB7240FB445

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

#endif /* F5237355_DFED_4CDA_BBFF_AAB7240FB445 */
