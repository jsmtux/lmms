#ifndef BC6C36FA_574A_4A42_9A38_2FDF9DA7DA44
#define BC6C36FA_574A_4A42_9A38_2FDF9DA7DA44

#include "AudioDeviceSetupWidget.h"

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
        return QT_TRANSLATE_NOOP( "AudioDeviceSetupWidget", "Dummy (no sound output)" );
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

#endif /* BC6C36FA_574A_4A42_9A38_2FDF9DA7DA44 */
