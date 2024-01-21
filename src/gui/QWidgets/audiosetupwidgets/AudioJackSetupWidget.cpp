#include "AudioJackSetupWidget.h"

#include <QLabel>

#include "IConfigManager.h"
#include "widgets/LcdSpinBox.h"

#include "gui_templates.h"

namespace lmms {

AudioJackSetupWidget::AudioJackSetupWidget( QWidget * _parent ) :
	AudioDeviceSetupWidget( AudioJackSetupWidget::name(), _parent )
{
	QString cn = IConfigManager::Instance()->value( "audiojack", "clientname" );
	if( cn.isEmpty() )
	{
		cn = "lmms";
	}
	m_clientName = new QLineEdit( cn, this );
	m_clientName->setGeometry( 10, 20, 160, 20 );

	auto cn_lbl = new QLabel(tr("Client name"), this);
	cn_lbl->setFont( pointSize<7>( cn_lbl->font() ) );
	cn_lbl->setGeometry( 10, 40, 160, 10 );

	auto m = MFact::createIntModel(DEFAULT_CHANNELS, SURROUND_CHANNELS, 2, nullptr, "");
	m->setValue( IConfigManager::Instance()->value( "audiojack",
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
	IConfigManager::Instance()->setValue( "audiojack", "clientname",
							m_clientName->text() );
	IConfigManager::Instance()->setValue( "audiojack", "channels",
				QString::number( m_channels->model()->value() ) );
}

}