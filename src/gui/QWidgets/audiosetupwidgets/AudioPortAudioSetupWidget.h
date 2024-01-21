#ifndef LMMS_GUI_AUDIOPORTAUDIOSETUPWIDGET_H
#define LMMS_GUI_AUDIOPORTAUDIOSETUPWIDGET_H

#include "lmmsconfig.h"

#ifdef LMMS_HAVE_PORTAUDIO

#include "AudioDeviceSetupWidget.h"
#include "widgets/ComboBox.h"

namespace lmms
{

class AudioPortAudioSetupUtil : public QObject
{
Q_OBJECT
public:
    AudioPortAudioSetupUtil();
public slots:
	void updateBackends();
	void updateDevices();
	void updateChannels();

public:
	std::unique_ptr<IComboBoxModelWrapper> m_backendModel;
	std::unique_ptr<IComboBoxModelWrapper> m_deviceModel;
};

namespace gui
{

class AudioPortAudioSetupWidget : public AudioDeviceSetupWidget
{
public:
    AudioPortAudioSetupWidget( QWidget * _parent );
    ~AudioPortAudioSetupWidget() override;

    void saveSettings() override;
    void show() override;

private:
    gui::ComboBox * m_backend;
    gui::ComboBox * m_device;
    AudioPortAudioSetupUtil m_setupUtil;

} ;

}
}

#endif // LMMS_HAVE_PORTAUDIO

#endif // LMMS_GUI_AUDIOPORTAUDIOSETUPWIDGET_H
