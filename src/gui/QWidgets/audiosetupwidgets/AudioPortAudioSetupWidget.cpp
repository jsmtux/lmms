#include "AudioPortAudioSetupWidget.h"


#ifdef LMMS_HAVE_PORTAUDIO

#include <portaudio.h>
#include "IAudioDevice.h"
#include "IConfigManager.h"
#include "gui_templates.h"

#include <QLabel>

namespace lmms
{


AudioPortAudioSetupUtil::AudioPortAudioSetupUtil()
	: m_backendModel(MFact::createComboBox(this))
	, m_deviceModel(MFact::createComboBox(this))
{}


void AudioPortAudioSetupUtil::updateBackends()
{
	PaError err = Pa_Initialize();
	if( err != paNoError ) {
		printf( "Couldn't initialize PortAudio: %s\n", Pa_GetErrorText( err ) );
		return;
	}

	const PaHostApiInfo * hi;
	for( int i = 0; i < Pa_GetHostApiCount(); ++i )
	{
		hi = Pa_GetHostApiInfo( i );
		m_backendModel->addItem( hi->name );
	}

	Pa_Terminate();
}




void AudioPortAudioSetupUtil::updateDevices()
{
	PaError err = Pa_Initialize();
	if( err != paNoError ) {
		printf( "Couldn't initialize PortAudio: %s\n", Pa_GetErrorText( err ) );
		return;
	}

	// get active backend 
	const QString& backend = m_backendModel->currentText();
	int hostApi = 0;
	const PaHostApiInfo * hi;
	for( int i = 0; i < Pa_GetHostApiCount(); ++i )
	{
		hi = Pa_GetHostApiInfo( i );
		if( backend == hi->name )
		{
			hostApi = i;
			break;
		}
	}

	// get devices for selected backend
	m_deviceModel->clear();
	const PaDeviceInfo * di;
	for( int i = 0; i < Pa_GetDeviceCount(); ++i )
	{
		di = Pa_GetDeviceInfo( i );
		if( di->hostApi == hostApi )
		{
			m_deviceModel->addItem( di->name );
		}
	}
	Pa_Terminate();
}




void AudioPortAudioSetupUtil::updateChannels()
{
	PaError err = Pa_Initialize();
	if( err != paNoError ) {
		printf( "Couldn't initialize PortAudio: %s\n", Pa_GetErrorText( err ) );
		return;
	}
	// get active backend 
	Pa_Terminate();
}

}

namespace lmms::gui
{

AudioPortAudioSetupWidget::AudioPortAudioSetupWidget( QWidget * _parent ) :
	AudioDeviceSetupWidget( PortAudioName(), _parent )
{
	using gui::ComboBox;

	m_backend = new ComboBox( m_setupUtil.m_backendModel.get(), this, "BACKEND" );
	m_backend->setGeometry( 64, 15, 260, ComboBox::DEFAULT_HEIGHT );

	auto backend_lbl = new QLabel(tr("Backend"), this);
	backend_lbl->setFont( pointSize<7>( backend_lbl->font() ) );
	backend_lbl->move( 8, 18 );

	m_device = new ComboBox( m_setupUtil.m_deviceModel.get(), this, "DEVICE" );
	m_device->setGeometry( 64, 35, 260, ComboBox::DEFAULT_HEIGHT );

	auto dev_lbl = new QLabel(tr("Device"), this);
	dev_lbl->setFont( pointSize<7>( dev_lbl->font() ) );
	dev_lbl->move( 8, 38 );
	
/*	LcdSpinBoxModel * m = new LcdSpinBoxModel(  );
	m->setRange( DEFAULT_CHANNELS, SURROUND_CHANNELS );
	m->setStep( 2 );
	m->setValue( IConfigManager::Instance()->value( "audioportaudio",
							"channels" ).toInt() );

	m_channels = new LcdSpinBox( 1, this );
	m_channels->setModel( m );
	m_channels->setLabel( tr( "Channels" ) );
	m_channels->move( 308, 20 );*/

	connect( m_setupUtil.m_backendModel->wrappedModel()->model(), &Model::dataChanged,
			&m_setupUtil, &AudioPortAudioSetupUtil::updateDevices);
			
	connect( m_setupUtil.m_deviceModel->wrappedModel()->model(), &Model::dataChanged,
			&m_setupUtil, &AudioPortAudioSetupUtil::updateChannels);
}




AudioPortAudioSetupWidget::~AudioPortAudioSetupWidget()
{
	disconnect( m_setupUtil.m_backendModel->wrappedModel()->model(), &Model::dataChanged,
			&m_setupUtil, &AudioPortAudioSetupUtil::updateDevices);
			
	disconnect( m_setupUtil.m_deviceModel->wrappedModel()->model(), &Model::dataChanged,
			&m_setupUtil, &AudioPortAudioSetupUtil::updateChannels);
}




void AudioPortAudioSetupWidget::saveSettings()
{

	IConfigManager::Instance()->setValue( "audioportaudio", "backend",
							m_setupUtil.m_backendModel->currentText() );
	IConfigManager::Instance()->setValue( "audioportaudio", "device",
							m_setupUtil.m_deviceModel->currentText() );
/*	IConfigManager::Instance()->setValue( "audioportaudio", "channels",
				QString::number( m_channels->value<int>() ) );*/

}




void AudioPortAudioSetupWidget::show()
{
	if( m_setupUtil.m_backendModel->size() == 0 )
	{
		// populate the backend model the first time we are shown
		m_setupUtil.updateBackends();

		const QString& backend = IConfigManager::Instance()->value(
			"audioportaudio", "backend" );
		const QString& device = IConfigManager::Instance()->value(
			"audioportaudio", "device" );
		
		int i = qMax( 0, m_setupUtil.m_backendModel->findText( backend ) );
		m_setupUtil.m_backendModel->setValue( i );
		
		m_setupUtil.updateDevices();
		
		i = qMax( 0, m_setupUtil.m_deviceModel->findText( device ) );
		m_setupUtil.m_deviceModel->setValue( i );
	}

	AudioDeviceSetupWidget::show();
}



}

#endif // LMMS_HAVE_PORTAUDIO
