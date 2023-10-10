#include "AudioJackSetupWidget.h"

#include <QLabel>

#include "ConfigManager.h"

#include "gui_templates.h"

namespace lmms {

AudioJackSetupWidget::AudioJackSetupWidget( QWidget * _parent ) :
	AudioDeviceSetupWidget( AudioJackSetupWidget::name(), _parent )
{
	QString cn = ConfigManager::inst()->value( "audiojack", "clientname" );
	if( cn.isEmpty() )
	{
		cn = "lmms";
	}
	m_clientName = new QLineEdit( cn, this );
	m_clientName->setGeometry( 10, 20, 160, 20 );

	auto cn_lbl = new QLabel(tr("Client name"), this);
	cn_lbl->setFont( pointSize<7>( cn_lbl->font() ) );
	cn_lbl->setGeometry( 10, 40, 160, 10 );

	auto m = new gui::LcdSpinBoxModel(/* this */);
	m->setRange( DEFAULT_CHANNELS, SURROUND_CHANNELS );
	m->setStep( 2 );
	m->setValue( ConfigManager::inst()->value( "audiojack",
							"channels" ).toInt() );

	m_channels = new gui::LcdSpinBox( 1, m, this );
	m_channels->setLabel( tr( "Channels" ) );
	m_channels->move( 180, 20 );

}

AudioJackSetupWidget::~AudioJackSetupWidget()
{
	delete m_channels->model();
}

void AudioJackSetupWidget::saveSettings()
{
	ConfigManager::inst()->setValue( "audiojack", "clientname",
							m_clientName->text() );
	ConfigManager::inst()->setValue( "audiojack", "channels",
				QString::number( m_channels->value<int>() ) );
}

}