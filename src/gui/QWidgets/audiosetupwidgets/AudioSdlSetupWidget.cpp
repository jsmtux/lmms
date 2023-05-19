
#include "AudioSdlSetupWidget.h"

#ifdef LMMS_HAVE_SDL
#include "AudioSdl.h"
#include "ConfigManager.h"
#include "gui_templates.h"

#include <QLabel>

namespace lmms {

AudioSdlSetupWidget::AudioSdlSetupWidget( QWidget * _parent ) :
	AudioDeviceSetupWidget( AudioSdlSetupWidget::name(), _parent )
{
	QString dev = ConfigManager::inst()->value( "audiosdl", "device" );
	m_device = new QLineEdit( dev, this );
	m_device->setGeometry( 10, 20, 160, 20 );

	auto dev_lbl = new QLabel(tr("Device"), this);
	dev_lbl->setFont( pointSize<7>( dev_lbl->font() ) );
	dev_lbl->setGeometry( 10, 40, 160, 10 );

}

void AudioSdlSetupWidget::saveSettings()
{
	ConfigManager::inst()->setValue( "audiosdl", "device",
							m_device->text() );
}

}

#endif
