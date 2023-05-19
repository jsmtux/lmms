#include "AudioPulseAudioSetupWidget.h"

#include <QLabel>

#include "AudioPulseAudio.h"
#include "ConfigManager.h"
#include "gui_templates.h"

namespace lmms {

AudioPulseAudioSetupWidget::AudioPulseAudioSetupWidget( QWidget * _parent ) :
	AudioDeviceSetupWidget( AudioPulseAudioSetupWidget::name(), _parent )
{
	m_device = new QLineEdit( AudioPulseAudio::probeDevice(), this );
	m_device->setGeometry( 10, 20, 160, 20 );

	auto dev_lbl = new QLabel(tr("Device"), this);
	dev_lbl->setFont( pointSize<7>( dev_lbl->font() ) );
	dev_lbl->setGeometry( 10, 40, 160, 10 );

	auto m = new gui::LcdSpinBoxModel(/* this */);
	m->setRange( DEFAULT_CHANNELS, SURROUND_CHANNELS );
	m->setStep( 2 );
	m->setValue( ConfigManager::inst()->value( "audiopa",
							"channels" ).toInt() );

	m_channels = new gui::LcdSpinBox( 1, this );
	m_channels->setModel( m );
	m_channels->setLabel( tr( "Channels" ) );
	m_channels->move( 180, 20 );

}


AudioPulseAudioSetupWidget::~AudioPulseAudioSetupWidget()
{
	delete m_channels->model();
}


void AudioPulseAudioSetupWidget::saveSettings()
{
	ConfigManager::inst()->setValue( "audiopa", "device",
							m_device->text() );
	ConfigManager::inst()->setValue( "audiopa", "channels",
				QString::number( m_channels->value<int>() ) );
}

}
