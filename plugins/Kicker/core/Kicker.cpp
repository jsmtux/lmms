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
#include "IAudioEngine.h"
#include "IEngine.h"
#include "ITrack.h"
#include "IPlayHandle.h"
#include "KickerOsc.h"

#include "embed.h"

namespace lmms
{

extern "C"
{

PluginDescriptor kicker_plugin_descriptor =
{
	LMMS_STRINGIFY( PLUGIN_NAME ),
	KickerInstrument::PluginName(),
	QT_TRANSLATE_NOOP( "PluginBrowser",
				"Versatile drum synthesizer" ),
	"Tobias Doerffel <tobydox/at/users.sf.net>",
	0x0100,
	PluginTypes::Instrument,
	nullptr,
	nullptr,
	nullptr,
} ;

}


KickerInstrument::KickerInstrument( IInstrumentTrack * _instrument_track ) :
	Instrument( _instrument_track, &kicker_plugin_descriptor ),
	m_startFreqModel( MFact::create(150.0f, 5.0f, 1000.0f, 1.0f, this, tr( "Start frequency" ) )),
	m_endFreqModel( MFact::create(40.0f, 5.0f, 1000.0f, 1.0f, this, tr( "End frequency" ) )),
	m_decayModel( MFact::create(440.0f, 5.0f, 5000.0f, 1.0f, 5000.0f, this, tr( "Length" ) )),
	m_distModel( MFact::create(0.8f, 0.0f, 100.0f, 0.1f, this, tr( "Start distortion" ) )),
	m_distEndModel( MFact::create(0.8f, 0.0f, 100.0f, 0.1f, this, tr( "End distortion" ) )),
	m_gainModel( MFact::create(1.0f, 0.1f, 5.0f, 0.05f, this, tr( "Gain" ) )),
	m_envModel( MFact::create(0.163f, 0.01f, 1.0f, 0.001f, this, tr( "Envelope slope" ) )),
	m_noiseModel( MFact::create(0.0f, 0.0f, 1.0f, 0.01f, this, tr( "Noise" ) )),
	m_clickModel( MFact::create(0.4f, 0.0f, 1.0f, 0.05f, this, tr( "Click" ) )),
	m_slopeModel( MFact::create(0.06f, 0.001f, 1.0f, 0.001f, this, tr( "Frequency slope" ) )),
	m_startNoteModel( MFact::create(true, this, tr( "Start from note" )) ),
	m_endNoteModel( MFact::create(false, this, tr( "End to note" )) ),
	m_versionModel( MFact::createIntModel(KICKER_PRESET_VERSION, 0, KICKER_PRESET_VERSION, this, "" )),
	m_instrument_track(_instrument_track)
{
}




void KickerInstrument::saveSettings( QDomDocument & _doc,
							QDomElement & _this )
{
	m_startFreqModel->saveSettings( _doc, _this, "startfreq" );
	m_endFreqModel->saveSettings( _doc, _this, "endfreq" );
	m_decayModel->saveSettings( _doc, _this, "decay" );
	m_distModel->saveSettings( _doc, _this, "dist" );
	m_distEndModel->saveSettings( _doc, _this, "distend" );
	m_gainModel->saveSettings( _doc, _this, "gain" );
	m_envModel->saveSettings( _doc, _this, "env" );
	m_noiseModel->saveSettings( _doc, _this, "noise" );
	m_clickModel->saveSettings( _doc, _this, "click" );
	m_slopeModel->saveSettings( _doc, _this, "slope" );
	m_startNoteModel->saveSettings( _doc, _this, "startnote" );
	m_endNoteModel->saveSettings( _doc, _this, "endnote" );
	m_versionModel->saveSettings( _doc, _this, "version" );
}




void KickerInstrument::loadSettings( const QDomElement & _this )
{
	m_versionModel->loadSettings( _this, "version" );

	m_startFreqModel->loadSettings( _this, "startfreq" );
	m_endFreqModel->loadSettings( _this, "endfreq" );
	m_decayModel->loadSettings( _this, "decay" );
	m_distModel->loadSettings( _this, "dist" );
	if( _this.hasAttribute( "distend" ) )
	{
		m_distEndModel->loadSettings( _this, "distend" );
	}
	else
	{
		m_distEndModel->setValue( m_distModel->value() );
	}
	m_gainModel->loadSettings( _this, "gain" );
	m_envModel->loadSettings( _this, "env" );
	m_noiseModel->loadSettings( _this, "noise" );
	m_clickModel->loadSettings( _this, "click" );
	m_slopeModel->loadSettings( _this, "slope" );
	m_startNoteModel->loadSettings( _this, "startnote" );
	if( m_versionModel->value() < 1 )
	{
		m_startNoteModel->setValue( false );
	}
	m_endNoteModel->loadSettings( _this, "endnote" );

	// Try to maintain backwards compatibility
	if( !_this.hasAttribute( "version" ) )
	{
		m_startNoteModel->setValue( false );
		m_decayModel->wrappedModel()->setValue( m_decayModel->wrappedModel()->value() * 1.33f );
		m_envModel->setValue( 1.0f );
		m_slopeModel->setValue( 1.0f );
		m_clickModel->setValue( 0.0f );
	}
	m_versionModel->setValue( KICKER_PRESET_VERSION );
}




QString KickerInstrument::nodeName() const
{
	return PluginName();
}

using DistFX = DspEffectLibrary::Distortion;
using SweepOsc = KickerOsc<DspEffectLibrary::MonoToStereoAdaptor<DistFX>>;

void KickerInstrument::playNote( INotePlayHandle * _n,
						sampleFrame * _working_buffer )
{
	const fpp_t frames = _n->framesLeftForCurrentPeriod();
	const f_cnt_t offset = _n->noteOffset();
	const float decfr = m_decayModel->wrappedModel()->value() * IEngine::Instance()->getAudioEngineInterface()->processingSampleRate() / 1000.0f;
	const f_cnt_t tfp = _n->totalFramesPlayed();

	if ( tfp == 0 )
	{
		_n->setPluginData(new SweepOsc(
					DistFX( m_distModel->value(),
							m_gainModel->value() ),
					m_startNoteModel->value() ? _n->frequency() : m_startFreqModel->value(),
					m_endNoteModel->value() ? _n->frequency() : m_endFreqModel->value(),
					m_noiseModel->value() * m_noiseModel->value(),
					m_clickModel->value() * 0.25f,
					m_slopeModel->value(),
					m_envModel->value(),
					m_distModel->value(),
					m_distEndModel->value(),
					decfr ));
	}
	else if( tfp > decfr && !_n->isReleased() )
	{
		_n->noteOff();
	}

	auto so = static_cast<SweepOsc*>(_n->pluginData());
	so->update( _working_buffer + offset, frames, IEngine::Instance()->getAudioEngineInterface()->processingSampleRate() );

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

	m_instrument_track->processAudioBuffer( _working_buffer, frames + offset, _n );
}




void KickerInstrument::deleteNotePluginData( INotePlayHandle * _n )
{
	delete static_cast<SweepOsc *>( _n->pluginData() );
}



} // namespace lmms
