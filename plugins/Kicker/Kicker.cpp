/*
 * Kicker.cpp - drum synthesizer
 *
 * Copyright (c) 2006-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2014 grejppi <grejppi/at/gmail.com>
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

#include "Kicker.h"
#include "AudioEngine.h"
#include "Engine.h"
#include "InstrumentTrack.h"
#include "NotePlayHandle.h"
#include "KickerOsc.h"

#include "embed.h"
#include "plugin_export.h"

#include "widgets/Knob.h"
#include "widgets/LedCheckBox.h"
#include "widgets/TempoSyncKnob.h"

namespace lmms
{


extern "C"
{

Plugin::Descriptor PLUGIN_EXPORT kicker_plugin_descriptor =
{
	LMMS_STRINGIFY( PLUGIN_NAME ),
	"Kicker",
	QT_TRANSLATE_NOOP( "PluginBrowser",
				"Versatile drum synthesizer" ),
	"Tobias Doerffel <tobydox/at/users.sf.net>",
	0x0100,
	Plugin::Instrument,
	new PluginPixmapLoader( "logo" ),
	nullptr,
	nullptr,
} ;

}


KickerInstrument::KickerInstrument( InstrumentTrack * _instrument_track ) :
	QWidgetInstrumentPlugin( _instrument_track, &kicker_plugin_descriptor ),
	m_startFreqModel( 150.0f, 5.0f, 1000.0f, 1.0f, model(), tr( "Start frequency" ) ),
	m_endFreqModel( 40.0f, 5.0f, 1000.0f, 1.0f, model(), tr( "End frequency" ) ),
	m_decayModel( 440.0f, 5.0f, 5000.0f, 1.0f, 5000.0f, model(), tr( "Length" ) ),
	m_distModel( 0.8f, 0.0f, 100.0f, 0.1f, model(), tr( "Start distortion" ) ),
	m_distEndModel( 0.8f, 0.0f, 100.0f, 0.1f, model(), tr( "End distortion" ) ),
	m_gainModel( 1.0f, 0.1f, 5.0f, 0.05f, model(), tr( "Gain" ) ),
	m_envModel( 0.163f, 0.01f, 1.0f, 0.001f, model(), tr( "Envelope slope" ) ),
	m_noiseModel( 0.0f, 0.0f, 1.0f, 0.01f, model(), tr( "Noise" ) ),
	m_clickModel( 0.4f, 0.0f, 1.0f, 0.05f, model(), tr( "Click" ) ),
	m_slopeModel( 0.06f, 0.001f, 1.0f, 0.001f, model(), tr( "Frequency slope" ) ),
	m_startNoteModel( true, model(), tr( "Start from note" ) ),
	m_endNoteModel( false, model(), tr( "End to note" ) ),
	m_versionModel( KICKER_PRESET_VERSION, 0, KICKER_PRESET_VERSION, model(), "" )
{
}




void KickerInstrument::saveSettings( QDomDocument & _doc,
							QDomElement & _this )
{
	m_startFreqModel.saveSettings( _doc, _this, "startfreq" );
	m_endFreqModel.saveSettings( _doc, _this, "endfreq" );
	m_decayModel.saveSettings( _doc, _this, "decay" );
	m_distModel.saveSettings( _doc, _this, "dist" );
	m_distEndModel.saveSettings( _doc, _this, "distend" );
	m_gainModel.saveSettings( _doc, _this, "gain" );
	m_envModel.saveSettings( _doc, _this, "env" );
	m_noiseModel.saveSettings( _doc, _this, "noise" );
	m_clickModel.saveSettings( _doc, _this, "click" );
	m_slopeModel.saveSettings( _doc, _this, "slope" );
	m_startNoteModel.saveSettings( _doc, _this, "startnote" );
	m_endNoteModel.saveSettings( _doc, _this, "endnote" );
	m_versionModel.saveSettings( _doc, _this, "version" );
}




void KickerInstrument::loadSettings( const QDomElement & _this )
{
	m_versionModel.loadSettings( _this, "version" );

	m_startFreqModel.loadSettings( _this, "startfreq" );
	m_endFreqModel.loadSettings( _this, "endfreq" );
	m_decayModel.loadSettings( _this, "decay" );
	m_distModel.loadSettings( _this, "dist" );
	if( _this.hasAttribute( "distend" ) )
	{
		m_distEndModel.loadSettings( _this, "distend" );
	}
	else
	{
		m_distEndModel.setValue( m_distModel.value() );
	}
	m_gainModel.loadSettings( _this, "gain" );
	m_envModel.loadSettings( _this, "env" );
	m_noiseModel.loadSettings( _this, "noise" );
	m_clickModel.loadSettings( _this, "click" );
	m_slopeModel.loadSettings( _this, "slope" );
	m_startNoteModel.loadSettings( _this, "startnote" );
	if( m_versionModel.value() < 1 )
	{
		m_startNoteModel.setValue( false );
	}
	m_endNoteModel.loadSettings( _this, "endnote" );

	// Try to maintain backwards compatibility
	if( !_this.hasAttribute( "version" ) )
	{
		m_startNoteModel.setValue( false );
		m_decayModel.setValue( m_decayModel.value() * 1.33f );
		m_envModel.setValue( 1.0f );
		m_slopeModel.setValue( 1.0f );
		m_clickModel.setValue( 0.0f );
	}
	m_versionModel.setValue( KICKER_PRESET_VERSION );
}




QString KickerInstrument::nodeName() const
{
	return kicker_plugin_descriptor.name;
}

using DistFX = DspEffectLibrary::Distortion;
using SweepOsc = KickerOsc<DspEffectLibrary::MonoToStereoAdaptor<DistFX>>;

void KickerInstrument::playNote( NotePlayHandle * _n,
						sampleFrame * _working_buffer )
{
	const fpp_t frames = _n->framesLeftForCurrentPeriod();
	const f_cnt_t offset = _n->noteOffset();
	const float decfr = m_decayModel.value() * Engine::audioEngine()->processingSampleRate() / 1000.0f;
	const f_cnt_t tfp = _n->totalFramesPlayed();

	if ( tfp == 0 )
	{
		_n->m_pluginData = new SweepOsc(
					DistFX( m_distModel.value(),
							m_gainModel.value() ),
					m_startNoteModel.value() ? _n->frequency() : m_startFreqModel.value(),
					m_endNoteModel.value() ? _n->frequency() : m_endFreqModel.value(),
					m_noiseModel.value() * m_noiseModel.value(),
					m_clickModel.value() * 0.25f,
					m_slopeModel.value(),
					m_envModel.value(),
					m_distModel.value(),
					m_distEndModel.value(),
					decfr );
	}
	else if( tfp > decfr && !_n->isReleased() )
	{
		_n->noteOff();
	}

	auto so = static_cast<SweepOsc*>(_n->m_pluginData);
	so->update( _working_buffer + offset, frames, Engine::audioEngine()->processingSampleRate() );

	if( _n->isReleased() )
	{
		const float done = _n->releaseFramesDone();
		const float desired = desiredReleaseFrames();
		for( fpp_t f = 0; f < frames; ++f )
		{
			const float fac = ( done+f < desired ) ? ( 1.0f - ( ( done+f ) / desired ) ) : 0;
			_working_buffer[f+offset][0] *= fac;
			_working_buffer[f+offset][1] *= fac;
		}
	}

	instrumentTrack()->processAudioBuffer( _working_buffer, frames + offset, _n );
}




void KickerInstrument::deleteNotePluginData( NotePlayHandle * _n )
{
	delete static_cast<SweepOsc *>( _n->m_pluginData );
}




gui::InstrumentView * KickerInstrument::instantiateView( QWidget * _parent )
{
	return new gui::KickerInstrumentView( this, _parent );
}


namespace gui
{


class KickerKnob : public Knob
{
public:
	KickerKnob( FloatModel* _model, QWidget * _parent ) :
			Knob( knobStyled, _model, _parent )
	{
		setFixedSize( 29, 29 );
		setObjectName( "smallKnob" );
	}
};


class KickerEnvKnob : public TempoSyncKnob
{
public:
	KickerEnvKnob( TempoSyncKnobModel* _model, QWidget * _parent ) :
			TempoSyncKnob( knobStyled, _model, _parent )
	{
		setFixedSize( 29, 29 );
		setObjectName( "smallKnob" );
	}
};


class KickerLargeKnob : public Knob
{
public:
	KickerLargeKnob( FloatModel* _model, QWidget * _parent ) :
			Knob( knobStyled, _model, _parent )
	{
		setFixedSize( 34, 34 );
		setObjectName( "largeKnob" );
	}
};




KickerInstrumentView::KickerInstrumentView( KickerInstrument * _instrument,
							QWidget * _parent ) :
	InstrumentViewImpl( _instrument, _parent, true )
{
	const int ROW1 = 14;
	const int ROW2 = ROW1 + 56;
	const int ROW3 = ROW2 + 56;
	const int LED_ROW = 63;
	const int COL1 = 14;
	const int COL2 = COL1 + 56;
	const int COL3 = COL2 + 56;
	const int COL4 = COL3 + 41;
	const int COL5 = COL4 + 41;
	const int END_COL = COL1 + 48;
	
	m_startFreqKnob = new KickerLargeKnob(  &m_instrument->m_startFreqModel, this );
	m_startFreqKnob->setHintText( tr( "Start frequency:" ), "Hz" );
	m_startFreqKnob->move( COL1, ROW1 );

	m_endFreqKnob = new KickerLargeKnob( &m_instrument->m_endFreqModel, this );
	m_endFreqKnob->setHintText( tr( "End frequency:" ), "Hz" );
	m_endFreqKnob->move( END_COL, ROW1 );

	m_slopeKnob = new KickerKnob( &m_instrument->m_slopeModel, this );
	m_slopeKnob->setHintText( tr( "Frequency slope:" ), "" );
	m_slopeKnob->move( COL3, ROW1 );

	m_gainKnob = new KickerKnob( &m_instrument->m_gainModel, this );
	m_gainKnob->setHintText( tr( "Gain:" ), "" );
	m_gainKnob->move( COL1, ROW3 );

	m_decayKnob = new KickerEnvKnob( &m_instrument->m_decayModel, this );
	m_decayKnob->setHintText( tr( "Envelope length:" ), "ms" );
	m_decayKnob->move( COL2, ROW3 );

	m_envKnob = new KickerKnob( &m_instrument->m_envModel, this );
	m_envKnob->setHintText( tr( "Envelope slope:" ), "" );
	m_envKnob->move( COL3, ROW3 );

	m_clickKnob = new KickerKnob( &m_instrument->m_clickModel, this );
	m_clickKnob->setHintText( tr( "Click:" ), "" );
	m_clickKnob->move( COL5, ROW1 );

	m_noiseKnob = new KickerKnob( &m_instrument->m_noiseModel, this );
	m_noiseKnob->setHintText( tr( "Noise:" ), "" );
	m_noiseKnob->move( COL5, ROW3 );

	m_distKnob = new KickerKnob( &m_instrument->m_distModel, this );
	m_distKnob->setHintText( tr( "Start distortion:" ), "" );
	m_distKnob->move( COL4, ROW2 );

	m_distEndKnob = new KickerKnob( &m_instrument->m_distEndModel, this );
	m_distEndKnob->setHintText( tr( "End distortion:" ), "" );
	m_distEndKnob->move( COL5, ROW2 );

	m_startNoteToggle = new LedCheckBox( "", &m_instrument->m_startNoteModel, this, "", LedCheckBox::Green );
	m_startNoteToggle->move( COL1 + 8, LED_ROW );

	m_endNoteToggle = new LedCheckBox( "", &m_instrument->m_endNoteModel, this, "", LedCheckBox::Green );
	m_endNoteToggle->move( END_COL + 8, LED_ROW );

	setAutoFillBackground( true );
	QPalette pal;
	pal.setBrush( backgroundRole(), PLUGIN_NAME::getIconPixmap( "artwork" ) );
	setPalette( pal );
}


} // namespace gui


extern "C"
{

// necessary for getting instance out of shared lib
PLUGIN_EXPORT Plugin * lmms_plugin_main( Model * m, void * )
{
	return new KickerInstrument( static_cast<InstrumentTrack *>( m ) );
}


}


} // namespace lmms
