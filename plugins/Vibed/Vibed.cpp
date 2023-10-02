/*
 * Vibed.cpp - combination of PluckedStringSynth and BitInvader
 *
 * Copyright (c) 2006-2008 Danny McRae <khjklujn/at/yahoo/com>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#include <QDomElement>

#include "Vibed.h"
#include "AudioEngine.h"
#include "Engine.h"
#include "InstrumentTrack.h"
#include "NotePlayHandle.h"
#include "base64.h"
#include "StringContainer.h"
#include "volume.h"
#include "Song.h"

#include "widgets/CaptionMenu.h"
#include "widgets/Knob.h"
#include "widgets/LedCheckBox.h"
#include "widgets/PixmapButton.h"

#include "embed.h"
#include "plugin_export.h"

namespace lmms
{


extern "C"
{

Plugin::Descriptor PLUGIN_EXPORT vibedstrings_plugin_descriptor =
{
	LMMS_STRINGIFY( PLUGIN_NAME ),
	"Vibed",
	QT_TRANSLATE_NOOP( "PluginBrowser",
					"Vibrating string modeler" ),
	"Danny McRae <khjklujn/at/yahoo/com>",
	0x0100,
	Plugin::Instrument,
	new PluginPixmapLoader( "logo" ),
	nullptr,
	nullptr,
};

}


Vibed::Vibed( InstrumentTrack * _instrumentTrack ) :
	QWidgetInstrumentPlugin( _instrumentTrack, &vibedstrings_plugin_descriptor )
{

	FloatModel * knob;
	BoolModel * led;
	gui::NineButtonSelectorModel * harmonic;
	gui::graphModel * graphTmp;

	for( int harm = 0; harm < 9; harm++ )
	{
		knob = new FloatModel( DefaultVolume, MinVolume, MaxVolume,
				1.0f, model(), tr( "String %1 volume" ).arg( harm+1 ) );
		m_volumeKnobs.append( knob );

		knob = new FloatModel( 0.0f, 0.0f, 0.05f, 0.001f, model(),
				tr( "String %1 stiffness" ).arg( harm+1 ) );
		m_stiffnessKnobs.append( knob );

		knob = new FloatModel( 0.0f, 0.0f, 0.05f, 0.005f, model(),
				tr( "Pick %1 position" ).arg( harm+1 )  );
		m_pickKnobs.append( knob );

		knob = new FloatModel( 0.05f, 0.0f, 0.05f, 0.005f, model(),
				tr( "Pickup %1 position" ).arg( harm+1 ) );
		m_pickupKnobs.append( knob );

		knob = new FloatModel( 0.0f, -1.0f, 1.0f, 0.01f, model(),
				tr( "String %1 panning" ).arg( harm+1 )  );
		m_panKnobs.append( knob );

		knob = new FloatModel( 0.0f, -0.1f, 0.1f, 0.001f, model(),
				tr( "String %1 detune" ).arg( harm+1 ) );
		m_detuneKnobs.append( knob );

		knob = new FloatModel( 0.0f, 0.0f, 0.75f, 0.01f, model(),
				tr( "String %1 fuzziness" ).arg( harm+1 ) );
		m_randomKnobs.append( knob );

		knob = new FloatModel( 1, 1, 16, 1, model(),
				tr( "String %1 length" ).arg( harm+1 ) );
		m_lengthKnobs.append( knob );

		led = new BoolModel( false, model(),
				tr( "Impulse %1" ).arg( harm+1 ) );
		m_impulses.append( led );

		led = new BoolModel( harm==0, model(),
				tr( "String %1" ).arg( harm+1 )  );
		m_powerButtons.append( led );

		harmonic = new gui::NineButtonSelectorModel( 2, 0, 8, model() );
		m_harmonics.append( harmonic );

		graphTmp = new gui::graphModel( -1.0, 1.0, __sampleLength, model() );
		graphTmp->setWaveToSine();

		m_graphs.append( graphTmp );

	}
}




void Vibed::saveSettings( QDomDocument & _doc, QDomElement & _this )
{

	QString name;
	
	// Save plugin version
	_this.setAttribute( "version", "0.1" );
	
	for( int i = 0; i < 9; i++ )
	{
		name = "active" + QString::number( i );
		_this.setAttribute( name, QString::number(
				m_powerButtons[i]->value() ) );

		if( m_powerButtons[i]->value() )
		{
			name = "volume" + QString::number( i );
			m_volumeKnobs[i]->saveSettings( _doc, _this, name );
	
			name = "stiffness" + QString::number( i );
			m_stiffnessKnobs[i]->saveSettings( _doc, _this, name );

			name = "pick" + QString::number( i );
			m_pickKnobs[i]->saveSettings( _doc, _this, name );

			name = "pickup" + QString::number( i );
			m_pickupKnobs[i]->saveSettings( _doc, _this, name );

			name = "octave" + QString::number( i );
			m_harmonics[i]->saveSettings( _doc, _this, name );

			name = "length" + QString::number( i );
			m_lengthKnobs[i]->saveSettings( _doc, _this, name );

			name = "pan" + QString::number( i );
			m_panKnobs[i]->saveSettings( _doc, _this, name );

			name = "detune" + QString::number( i );
			m_detuneKnobs[i]->saveSettings( _doc, _this, name );

			name = "slap" + QString::number( i );
			m_randomKnobs[i]->saveSettings( _doc, _this, name );

			name = "impulse" + QString::number( i );
			m_impulses[i]->saveSettings( _doc, _this, name );

			QString sampleString;
			base64::encode(
				(const char *)m_graphs[i]->samples(),
				__sampleLength * sizeof(float),
				sampleString );
			name = "graph" + QString::number( i );
			_this.setAttribute( name, sampleString );
		}
	}

}



void Vibed::loadSettings( const QDomElement & _this )
{

	QString name;

	for( int i = 0; i < 9; i++ )
	{
		name = "active" + QString::number( i );
		m_powerButtons[i]->setValue( _this.attribute( name ).toInt() );
		
		if( m_powerButtons[i]->value() &&
			_this.hasAttribute( "volume" + QString::number( i ) ) )
		{
			name = "volume" + QString::number( i );
			m_volumeKnobs[i]->loadSettings( _this, name );
		
			name = "stiffness" + QString::number( i );
			m_stiffnessKnobs[i]->loadSettings( _this, name );
		
			name = "pick" + QString::number( i );
			m_pickKnobs[i]->loadSettings( _this, name );
		
			name = "pickup" + QString::number( i );
			m_pickupKnobs[i]->loadSettings( _this, name );
		
			name = "octave" + QString::number( i );
			m_harmonics[i]->loadSettings( _this, name );
			
			name = "length" + QString::number( i );
			m_lengthKnobs[i]->loadSettings( _this, name );
		
			name = "pan" + QString::number( i );
			m_panKnobs[i]->loadSettings( _this, name );
		
			name = "detune" + QString::number( i );
			m_detuneKnobs[i]->loadSettings( _this, name );
		
			name = "slap" + QString::number( i );
			m_randomKnobs[i]->loadSettings( _this, name );
		
			name = "impulse" + QString::number( i );
			m_impulses[i]->loadSettings( _this, name );

			int size = 0;
			float * shp = 0;
			base64::decode( _this.attribute( "graph" +
						QString::number( i ) ),
						&shp,
						&size );
			// TODO: check whether size == 128 * sizeof( float ),
			// otherwise me might and up in a segfault
			m_graphs[i]->setSamples( shp );
			delete[] shp;
			

			// TODO: do one of the following to avoid
			// "uninitialized" wave-shape-buttongroup
			// - activate random-wave-shape-button here
			// - make wave-shape-buttons simple toggle-buttons
			//   instead of checkable buttons
			// - save and restore selected wave-shape-button
		}
	}
	
//	update();
}




QString Vibed::nodeName() const
{
	return( vibedstrings_plugin_descriptor.name );
}




void Vibed::playNote( NotePlayHandle * _n, sampleFrame * _working_buffer )
{
	if ( _n->totalFramesPlayed() == 0 || _n->m_pluginData == nullptr )
	{
		_n->m_pluginData = new StringContainer( _n->frequency(),
				Engine::audioEngine()->processingSampleRate(),
						__sampleLength );
		
		for( int i = 0; i < 9; ++i )
		{
			if( m_powerButtons[i]->value() )
			{
				static_cast<StringContainer*>(
					_n->m_pluginData )->addString(
				m_harmonics[i]->value(),
				m_pickKnobs[i]->value(),
				m_pickupKnobs[i]->value(),
				m_graphs[i]->samples(),
				m_randomKnobs[i]->value(),
				m_stiffnessKnobs[i]->value(),
				m_detuneKnobs[i]->value(),
				static_cast<int>(
					m_lengthKnobs[i]->value() ),
				m_impulses[i]->value(),
				i );
			}
		}
	}

	const fpp_t frames = _n->framesLeftForCurrentPeriod();
	const f_cnt_t offset = _n->noteOffset();
	auto ps = static_cast<StringContainer*>(_n->m_pluginData);

	for( fpp_t i = offset; i < frames + offset; ++i )
	{
		_working_buffer[i][0] = 0.0f;
		_working_buffer[i][1] = 0.0f;
		int s = 0;
		for( int string = 0; string < 9; ++string )
		{
			if( ps->exists( string ) )
			{
				// pan: 0 -> left, 1 -> right
				const float pan = ( m_panKnobs[string]->value() + 1 ) / 2.0f;
				const sample_t sample = ps->getStringSample( s ) * m_volumeKnobs[string]->value() / 100.0f;
				_working_buffer[i][0] += ( 1.0f - pan ) * sample;
				_working_buffer[i][1] += pan * sample;
				s++;
			}
		}
	}

	instrumentTrack()->processAudioBuffer( _working_buffer, frames + offset, _n );
}




void Vibed::deleteNotePluginData( NotePlayHandle * _n )
{
	delete static_cast<StringContainer *>( _n->m_pluginData );
}




gui::InstrumentView * Vibed::instantiateView( QWidget * _parent )
{
	return( new gui::VibedView( this, _parent ) );
}



namespace gui
{


VibedView::VibedView( Vibed * _instrument,
				QWidget * _parent ) :
	InstrumentViewImpl( _instrument, _parent, true )
{
	setAutoFillBackground( true );
	QPalette pal;
	pal.setBrush( backgroundRole(), PLUGIN_NAME::getIconPixmap(
								"artwork" ) );
	setPalette( pal );

	for(int i = 0; i < 9; i++)
	{		
		Knob* m_volumeKnob = new Knob( knobBright_26, m_instrument->m_volumeKnobs[i], this );
		m_volumeKnob->setVolumeKnob( true );
		m_volumeKnob->move( 103, 142 );
		m_volumeKnob->setHintText( tr( "String volume:" ), "" );
		m_volumeKnob->setVisible(i == 0);
		m_volumeKnobs.append(m_volumeKnob);

		Knob* m_stiffnessKnob = new Knob( knobBright_26, m_instrument->m_stiffnessKnobs[i], this );
		m_stiffnessKnob->move( 129, 142 );
		m_stiffnessKnob->setHintText( tr( "String stiffness:" )
								, "" );
		m_stiffnessKnob->setVisible(i == 0);
		m_stiffnessKnobs.append(m_stiffnessKnob);

		Knob* m_pickKnob = new Knob( knobBright_26, m_instrument->m_pickKnobs[i], this );
		m_pickKnob->move( 153, 142 );
		m_pickKnob->setHintText( tr( "Pick position:" ), "" );
		m_pickKnob->setVisible(i == 0);
		m_pickKnobs.append(m_pickKnob);

		Knob* m_pickupKnob = new Knob( knobBright_26, m_instrument->m_pickupKnobs[i], this );
		m_pickupKnob->move( 177, 142 );
		m_pickupKnob->setHintText( tr( "Pickup position:" )
							, "" );
		m_pickupKnob->setVisible(i == 0);
		m_pickupKnobs.append(m_pickupKnob);

		Knob* m_panKnob = new Knob( knobBright_26, m_instrument->m_panKnobs[i], this );
		m_panKnob->move( 105, 187 );
		m_panKnob->setHintText( tr( "String panning:" ), "" );
		m_panKnob->setVisible(i == 0);
		m_panKnobs.append(m_panKnob);
		
		Knob* m_detuneKnob = new Knob( knobBright_26, m_instrument->m_detuneKnobs[i], this );
		m_detuneKnob->move( 150, 187 );
		m_detuneKnob->setHintText( tr( "String detune:" ), "" );
		m_detuneKnob->setVisible(i == 0);
		m_detuneKnobs.append(m_detuneKnob);

		Knob* m_randomKnob = new Knob( knobBright_26, m_instrument->m_randomKnobs[i], this );
		m_randomKnob->move( 194, 187 );
		m_randomKnob->setHintText( tr( "String fuzziness:" )
							, "" );
		m_randomKnob->setVisible(i == 0);
		m_randomKnobs.append(m_randomKnob);

		Knob* m_lengthKnob = new Knob( knobBright_26, m_instrument->m_lengthKnobs[i], this );
		m_lengthKnob->move( 23, 193 );
		m_lengthKnob->setHintText( tr( "String length:" )
							, "" );
		m_lengthKnob->setVisible(i == 0);
		m_lengthKnobs.append(m_lengthKnob);

		LedCheckBox* m_impulse = new LedCheckBox( "", m_instrument->m_impulses[i], this );
		m_impulse->move( 23, 94 );
		m_impulse->setToolTip(
				tr( "Impulse" ) );
		m_impulse->setVisible(i == 0);
		m_impulses.append(m_impulse);

		NineButtonSelector* m_harmonic = new NineButtonSelector(
			PLUGIN_NAME::getIconPixmap( "button_-2_on" ),
			PLUGIN_NAME::getIconPixmap( "button_-2_off" ),
			PLUGIN_NAME::getIconPixmap( "button_-1_on" ),
			PLUGIN_NAME::getIconPixmap( "button_-1_off" ),
			PLUGIN_NAME::getIconPixmap( "button_f_on" ),
			PLUGIN_NAME::getIconPixmap( "button_f_off" ),
			PLUGIN_NAME::getIconPixmap( "button_2_on" ),
			PLUGIN_NAME::getIconPixmap( "button_2_off" ),
			PLUGIN_NAME::getIconPixmap( "button_3_on" ),
			PLUGIN_NAME::getIconPixmap( "button_3_off" ),
			PLUGIN_NAME::getIconPixmap( "button_4_on" ),
			PLUGIN_NAME::getIconPixmap( "button_4_off" ),
			PLUGIN_NAME::getIconPixmap( "button_5_on" ),
			PLUGIN_NAME::getIconPixmap( "button_5_off" ),
			PLUGIN_NAME::getIconPixmap( "button_6_on" ),
			PLUGIN_NAME::getIconPixmap( "button_6_off" ),
			PLUGIN_NAME::getIconPixmap( "button_7_on" ),
			PLUGIN_NAME::getIconPixmap( "button_7_off" ),
			2,
			21, 127, m_instrument->m_harmonics[i],
			this );

		m_harmonic->setWindowTitle( tr( "Octave" ) );
		m_harmonic->setVisible(i == 0);
		m_harmonics.append(m_harmonic);

		Graph* m_graph = new Graph( m_instrument->m_graphs[i], this );
		m_graph->setWindowTitle( tr( "Impulse Editor" ) );
		m_graph->setForeground( PLUGIN_NAME::getIconPixmap( "wavegraph4" ) );
		m_graph->move( 76, 21 );
		m_graph->resize(132, 104);
		m_graph->setVisible(i == 0);
		m_graphs.append(m_graph);


		LedCheckBox* m_power = new LedCheckBox( "", m_instrument->m_powerButtons[i], this, tr( "Enable waveform" ) );
		m_power->move( 212, 130 );
		m_power->setToolTip(
				tr( "Enable/disable string" ) );
		m_power->setVisible(i == 0);
		m_powers.append(m_power);

		m_prevString = 0;
	}
	

	m_stringSelector = new NineButtonSelector(
			PLUGIN_NAME::getIconPixmap( "button_1_on" ),
			PLUGIN_NAME::getIconPixmap( "button_1_off" ),
			PLUGIN_NAME::getIconPixmap( "button_2_on" ),
			PLUGIN_NAME::getIconPixmap( "button_2_off" ),
			PLUGIN_NAME::getIconPixmap( "button_3_on" ),
			PLUGIN_NAME::getIconPixmap( "button_3_off" ),
			PLUGIN_NAME::getIconPixmap( "button_4_on" ),
			PLUGIN_NAME::getIconPixmap( "button_4_off" ),
			PLUGIN_NAME::getIconPixmap( "button_5_on" ),
			PLUGIN_NAME::getIconPixmap( "button_5_off" ),
			PLUGIN_NAME::getIconPixmap( "button_6_on" ),
			PLUGIN_NAME::getIconPixmap( "button_6_off" ),
			PLUGIN_NAME::getIconPixmap( "button_7_on" ),
			PLUGIN_NAME::getIconPixmap( "button_7_off" ),
			PLUGIN_NAME::getIconPixmap( "button_8_on" ),
			PLUGIN_NAME::getIconPixmap( "button_8_off" ),
			PLUGIN_NAME::getIconPixmap( "button_9_on" ),
			PLUGIN_NAME::getIconPixmap( "button_9_off" ),
			0,
			21, 39,
			new NineButtonSelectorModel(0, 8, 0, nullptr, QString() ),
			this);

	
	// String selector is not a part of the model
	m_stringSelector->setWindowTitle( tr( "String" ) );

	connect( m_stringSelector, SIGNAL( NineButtonSelection( int ) ),
			this, SLOT( showString( int ) ) );

	m_sinWaveBtn = new PixmapButton( this, new BoolModel(false, this), tr( "Sine wave" ) );
	m_sinWaveBtn->move( 212, 24 );
	m_sinWaveBtn->setActiveGraphic( embed::getIconPixmap(
				"sin_wave_active" ) );
	m_sinWaveBtn->setInactiveGraphic( embed::getIconPixmap(
				"sin_wave_inactive" ) );
	m_sinWaveBtn->setToolTip(
				tr( "Sine wave" ) );
	connect( m_sinWaveBtn, SIGNAL (clicked () ),
			this, SLOT ( sinWaveClicked() ) );

	
	m_triangleWaveBtn = new PixmapButton( this, new BoolModel(false, this), tr( "Triangle wave" ) );
	m_triangleWaveBtn->move( 212, 41 );
	m_triangleWaveBtn->setActiveGraphic(
			embed::getIconPixmap( "triangle_wave_active" ) );
	m_triangleWaveBtn->setInactiveGraphic(
			embed::getIconPixmap( "triangle_wave_inactive" ) );
	m_triangleWaveBtn->setToolTip(
			tr( "Triangle wave" ) );
	connect( m_triangleWaveBtn, SIGNAL ( clicked () ),
			this, SLOT ( triangleWaveClicked() ) );

	
	m_sawWaveBtn = new PixmapButton( this, new BoolModel(false, this), tr( "Saw wave" ) );
	m_sawWaveBtn->move( 212, 58 );
	m_sawWaveBtn->setActiveGraphic( embed::getIconPixmap(
				"saw_wave_active" ) );
	m_sawWaveBtn->setInactiveGraphic( embed::getIconPixmap(
				"saw_wave_inactive" ) );
	m_sawWaveBtn->setToolTip(
				tr( "Saw wave" ) );
	connect( m_sawWaveBtn, SIGNAL (clicked () ),
			this, SLOT ( sawWaveClicked() ) );

	
	m_sqrWaveBtn = new PixmapButton( this, new BoolModel(false, this), tr( "Square wave" ) );
	m_sqrWaveBtn->move( 212, 75 );
	m_sqrWaveBtn->setActiveGraphic( embed::getIconPixmap(
				"square_wave_active" ) );
	m_sqrWaveBtn->setInactiveGraphic( embed::getIconPixmap(
				"square_wave_inactive" ) );
	m_sqrWaveBtn->setToolTip(
			tr( "Square wave" ) );
	connect( m_sqrWaveBtn, SIGNAL ( clicked () ),
			this, SLOT ( sqrWaveClicked() ) );

	
	m_whiteNoiseWaveBtn = new PixmapButton( this, new BoolModel(false, this), tr( "White noise" ) );
	m_whiteNoiseWaveBtn->move( 212, 92 );
	m_whiteNoiseWaveBtn->setActiveGraphic(
			embed::getIconPixmap( "white_noise_wave_active" ) );
	m_whiteNoiseWaveBtn->setInactiveGraphic(
			embed::getIconPixmap( "white_noise_wave_inactive" ) );
	m_whiteNoiseWaveBtn->setToolTip(
			tr( "White noise" ) );
	connect( m_whiteNoiseWaveBtn, SIGNAL ( clicked () ),
			this, SLOT ( noiseWaveClicked() ) );

	
	m_usrWaveBtn = new PixmapButton( this, new BoolModel(false, this), tr( "User-defined wave" ) );
	m_usrWaveBtn->move( 212, 109 );
	m_usrWaveBtn->setActiveGraphic( embed::getIconPixmap(
				"usr_wave_active" ) );
	m_usrWaveBtn->setInactiveGraphic( embed::getIconPixmap(
				"usr_wave_inactive" ) );
	m_usrWaveBtn->setToolTip(
			tr( "User-defined wave" ) );
	connect( m_usrWaveBtn, SIGNAL ( clicked () ),
			this, SLOT ( usrWaveClicked() ) );


	m_smoothBtn = new PixmapButton( this, new BoolModel(false, this), tr( "Smooth waveform" ) );
	m_smoothBtn->move( 79, 129 );
	m_smoothBtn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
			"smooth_active" ) );
	m_smoothBtn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
			"smooth_inactive" ) );
	m_smoothBtn->setChecked( false );
	m_smoothBtn->setToolTip(
			tr( "Smooth waveform" ) );
	connect( m_smoothBtn, SIGNAL ( clicked () ),
			this, SLOT ( smoothClicked() ) );
	
	m_normalizeBtn = new PixmapButton( this, new BoolModel(false, this), tr( "Normalize waveform" ) );
	m_normalizeBtn->move( 96, 129 );
	m_normalizeBtn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
			"normalize_active" ) );
	m_normalizeBtn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
			"normalize_inactive" ) );
	m_normalizeBtn->setChecked( false );
	m_normalizeBtn->setToolTip(
			tr( "Normalize waveform" ) );

	connect( m_normalizeBtn, SIGNAL ( clicked () ),
			this, SLOT ( normalizeClicked() ) );
}




void VibedView::showString( int _string )
{
	m_volumeKnobs[m_prevString]->setVisible(false);
	m_stiffnessKnobs[m_prevString]->setVisible(false);
	m_pickKnobs[m_prevString]->setVisible(false);
	m_pickupKnobs[m_prevString]->setVisible(false);
	m_panKnobs[m_prevString]->setVisible(false);
	m_detuneKnobs[m_prevString]->setVisible(false);
	m_randomKnobs[m_prevString]->setVisible(false);
	m_lengthKnobs[m_prevString]->setVisible(false);
	m_impulses[m_prevString]->setVisible(false);
	m_harmonics[m_prevString]->setVisible(false);
	m_graphs[m_prevString]->setVisible(false);
	m_powers[m_prevString]->setVisible(false);
	m_prevString = _string;

	m_volumeKnobs[_string]->setVisible(true);
	m_stiffnessKnobs[_string]->setVisible(true);
	m_pickKnobs[_string]->setVisible(true);
	m_pickupKnobs[_string]->setVisible(true);
	m_panKnobs[_string]->setVisible(true);
	m_detuneKnobs[_string]->setVisible(true);
	m_randomKnobs[_string]->setVisible(true);
	m_lengthKnobs[_string]->setVisible(true);
	m_impulses[_string]->setVisible(true);
	m_harmonics[_string]->setVisible(true);
	m_graphs[_string]->setVisible(true);
	m_powers[_string]->setVisible(true);
}




void VibedView::sinWaveClicked()
{
	m_graphs[m_prevString]->model()->setWaveToSine();
	Engine::getSong()->setModified();
}



void VibedView::triangleWaveClicked()
{
	m_graphs[m_prevString]->model()->setWaveToTriangle();
	Engine::getSong()->setModified();
}



void VibedView::sawWaveClicked()
{
	m_graphs[m_prevString]->model()->setWaveToSaw();
	Engine::getSong()->setModified();
}



void VibedView::sqrWaveClicked()
{
	m_graphs[m_prevString]->model()->setWaveToSquare();
	Engine::getSong()->setModified();
}



void VibedView::noiseWaveClicked()
{
	m_graphs[m_prevString]->model()->setWaveToNoise();
	Engine::getSong()->setModified();
}



void VibedView::usrWaveClicked()
{
	QString fileName = m_graphs[m_prevString]->model()->setWaveToUser();
	m_usrWaveBtn->setToolTip(fileName);
	Engine::getSong()->setModified();
}



void VibedView::smoothClicked()
{
	m_graphs[m_prevString]->model()->smooth();
	Engine::getSong()->setModified();
}



void VibedView::normalizeClicked()
{
	m_graphs[m_prevString]->model()->normalize();
	Engine::getSong()->setModified();
}




void VibedView::contextMenuEvent( QContextMenuEvent * )
{

	CaptionMenu contextMenu( m_instrument->model()->displayName(), this );
	contextMenu.exec( QCursor::pos() );

}


} // namespace gui

extern "C"
{

// necessary for getting instance out of shared lib
PLUGIN_EXPORT Plugin * lmms_plugin_main( Model *m, void * )
{
	return( new Vibed( static_cast<InstrumentTrack *>( m ) ) );
}


}


} // namespace lmms
