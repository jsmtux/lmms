/*
 * TripleOscillator.cpp - powerful instrument with three oscillators
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "TripleOscillator.h"
#include "IAudioEngine.h"
#include "debug.h"
#include "IEngine.h"
#include "ITrack.h"
#include "IPlayHandle.h"
#include "IOscillator.h"
#include "ISampleBuffer.h"

#include "embed.h"

namespace lmms
{


extern "C"
{

PluginDescriptor tripleoscillator_plugin_descriptor =
{
	LMMS_STRINGIFY( PLUGIN_NAME ),
	"TripleOscillator",
	QT_TRANSLATE_NOOP( "PluginBrowser",
				"Three powerful oscillators you can modulate "
				"in several ways" ),
	"Tobias Doerffel <tobydox/at/users.sf.net>",
	0x0110,
	PluginTypes::Instrument,
	new PluginPixmapLoader( "logo" ),
	nullptr,
	nullptr,
} ;

}



OscillatorObject::OscillatorObject( QObject * _parent, int _idx ) :
	Model( _parent ),
	m_volumeModel( MFact::create(DefaultVolume / NUM_OF_OSCILLATORS, MinVolume,
			MaxVolume, 1.0f, this, tr( "Osc %1 volume" ).arg( _idx+1 )) ),
	m_panModel( MFact::create(DefaultPanning, PanningLeft, PanningRight, 1.0f, this,
			tr( "Osc %1 panning" ).arg( _idx+1 )) ),
	m_coarseModel( MFact::create(-_idx*KeysPerOctave,
			-2 * KeysPerOctave, 2 * KeysPerOctave, 1.0f, this,
			tr( "Osc %1 coarse detuning" ).arg( _idx+1 )) ),
	m_fineLeftModel( MFact::create(0.0f, -100.0f, 100.0f, 1.0f, this,
			tr( "Osc %1 fine detuning left" ).arg( _idx+1 )) ),
	m_fineRightModel( MFact::create(0.0f, -100.0f, 100.0f, 1.0f, this,
			tr( "Osc %1 fine detuning right" ).arg( _idx + 1 )) ),
	m_phaseOffsetModel( MFact::create(0.0f, 0.0f, 360.0f, 1.0f, this,
			tr( "Osc %1 phase-offset" ).arg( _idx+1 )) ),
	m_stereoPhaseDetuningModel( MFact::create(0.0f, 0.0f, 360.0f, 1.0f, this,
			tr( "Osc %1 stereo phase-detuning" ).arg( _idx+1 )) ),
	m_waveShapeModel( MFact::createIntModel(IOscillator::SineWave, 0,
			IOscillator::NumWaveShapes-1, this,
			tr( "Osc %1 wave shape" ).arg( _idx+1 )) ),
	m_modulationAlgoModel( MFact::createIntModel(IOscillator::SignalMix, 0,
				IOscillator::NumModulationAlgos-1, this,
				tr( "Modulation type %1" ).arg( _idx+1 )) ),
	m_useWaveTableModel(MFact::create(true, this)),

	m_sampleBuffer( createSampleBuffer() ),
	m_volumeLeft( 0.0f ),
	m_volumeRight( 0.0f ),
	m_detuningLeft( 0.0f ),
	m_detuningRight( 0.0f ),
	m_phaseOffsetLeft( 0.0f ),
	m_phaseOffsetRight( 0.0f ),
	m_useWaveTable( true )
{
	// Connect knobs with Oscillators' inputs
	connect( m_volumeModel->model(), SIGNAL( dataChanged() ),
					this, SLOT( updateVolume() ), Qt::DirectConnection );
	connect( m_panModel->model(), SIGNAL( dataChanged() ),
					this, SLOT( updateVolume() ), Qt::DirectConnection );
	updateVolume();

	connect( m_coarseModel->model(), SIGNAL( dataChanged() ),
				this, SLOT( updateDetuningLeft() ), Qt::DirectConnection );
	connect( m_coarseModel->model(), SIGNAL( dataChanged() ),
				this, SLOT( updateDetuningRight() ), Qt::DirectConnection );
	connect( m_fineLeftModel->model(), SIGNAL( dataChanged() ),
				this, SLOT( updateDetuningLeft() ), Qt::DirectConnection );
	connect( m_fineRightModel->model(), SIGNAL( dataChanged() ),
				this, SLOT( updateDetuningRight() ), Qt::DirectConnection );
	updateDetuningLeft();
	updateDetuningRight();

	connect( m_phaseOffsetModel->model(), SIGNAL( dataChanged() ),
			this, SLOT( updatePhaseOffsetLeft() ), Qt::DirectConnection );
	connect( m_phaseOffsetModel->model(), SIGNAL( dataChanged() ),
			this, SLOT( updatePhaseOffsetRight() ), Qt::DirectConnection );
	connect( m_stereoPhaseDetuningModel->model(), SIGNAL( dataChanged() ),
			this, SLOT( updatePhaseOffsetLeft() ), Qt::DirectConnection );
	connect ( m_useWaveTableModel->model(), SIGNAL(dataChanged()),
			this, SLOT( updateUseWaveTable()));

	updatePhaseOffsetLeft();
	updatePhaseOffsetRight();

}




OscillatorObject::~OscillatorObject()
{
	// sharedObject::unref( m_sampleBuffer );
}




void OscillatorObject::oscUserDefWaveDblClick()
{
	QString af = m_sampleBuffer->openAndSetWaveformFile();
	if( af != "" )
	{
		// TODO:
		//m_usrWaveBtn->setToolTip(m_sampleBuffer->audioFile());
	}
}




void OscillatorObject::updateVolume()
{
	if( m_panModel->value() >= 0.0f )
	{
		const float panningFactorLeft = 1.0f - m_panModel->value()
							/ (float)PanningRight;
		m_volumeLeft = panningFactorLeft * m_volumeModel->value() /
									100.0f;
		m_volumeRight = m_volumeModel->value() / 100.0f;
	}
	else
	{
		m_volumeLeft = m_volumeModel->value() / 100.0f;
		const float panningFactorRight = 1.0f + m_panModel->value()
							/ (float)PanningRight;
		m_volumeRight = panningFactorRight * m_volumeModel->value() /
									100.0f;
	}
}




void OscillatorObject::updateDetuningLeft()
{
	m_detuningLeft = powf( 2.0f, ( (float)m_coarseModel->value() * 100.0f
				+ (float)m_fineLeftModel->value() ) / 1200.0f )
				/ IEngine::Instance()->getAudioEngineInterface()->processingSampleRate();
}




void OscillatorObject::updateDetuningRight()
{
	m_detuningRight = powf( 2.0f, ( (float)m_coarseModel->value() * 100.0f
				+ (float)m_fineRightModel->value() ) / 1200.0f )
				/ IEngine::Instance()->getAudioEngineInterface()->processingSampleRate();
}




void OscillatorObject::updatePhaseOffsetLeft()
{
	m_phaseOffsetLeft = ( m_phaseOffsetModel->value() +
				m_stereoPhaseDetuningModel->value() ) / 360.0f;
}




void OscillatorObject::updatePhaseOffsetRight()
{
	m_phaseOffsetRight = m_phaseOffsetModel->value() / 360.0f;
}

void OscillatorObject::updateUseWaveTable()
{
	m_useWaveTable = m_useWaveTableModel->value();
}




TripleOscillator::TripleOscillator( IInstrumentTrack * _instrument_track ) :
	Instrument( _instrument_track, &tripleoscillator_plugin_descriptor )
{
	for( int i = 0; i < NUM_OF_OSCILLATORS; ++i )
	{
		m_osc[i] = new OscillatorObject( this, i );

	}

	connect( IEngine::Instance()->getAudioEngineInterface(), SIGNAL( sampleRateChanged() ),
			this, SLOT( updateAllDetuning() ) );
}




void TripleOscillator::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	for( int i = 0; i < NUM_OF_OSCILLATORS; ++i )
	{
		QString is = QString::number( i );
		m_osc[i]->m_volumeModel->saveSettings( _doc, _this, "vol" + is );
		m_osc[i]->m_panModel->saveSettings( _doc, _this, "pan" + is );
		m_osc[i]->m_coarseModel->saveSettings( _doc, _this, "coarse"
									+ is );
		m_osc[i]->m_fineLeftModel->saveSettings( _doc, _this, "finel" +
									is );
		m_osc[i]->m_fineRightModel->saveSettings( _doc, _this, "finer" +
									is );
		m_osc[i]->m_phaseOffsetModel->saveSettings( _doc, _this,
							"phoffset" + is );
		m_osc[i]->m_stereoPhaseDetuningModel->saveSettings( _doc, _this,
							"stphdetun" + is );
		m_osc[i]->m_waveShapeModel->saveSettings( _doc, _this,
							"wavetype" + is );
		m_osc[i]->m_modulationAlgoModel->saveSettings( _doc, _this,
					"modalgo" + QString::number( i+1 ) );
		m_osc[i]->m_useWaveTableModel->saveSettings( _doc, _this,
					"useWaveTable" + QString::number (i+1 ) );
		_this.setAttribute( "userwavefile" + is,
					m_osc[i]->m_sampleBuffer->audioFile() );
	}
}




void TripleOscillator::loadSettings( const QDomElement & _this )
{
	for( int i = 0; i < NUM_OF_OSCILLATORS; ++i )
	{
		const QString is = QString::number( i );
		m_osc[i]->m_volumeModel->loadSettings( _this, "vol" + is );
		m_osc[i]->m_panModel->loadSettings( _this, "pan" + is );
		m_osc[i]->m_coarseModel->loadSettings( _this, "coarse" + is );
		m_osc[i]->m_fineLeftModel->loadSettings( _this, "finel" + is );
		m_osc[i]->m_fineRightModel->loadSettings( _this, "finer" + is );
		m_osc[i]->m_phaseOffsetModel->loadSettings( _this,
							"phoffset" + is );
		m_osc[i]->m_stereoPhaseDetuningModel->loadSettings( _this,
							"stphdetun" + is );
		m_osc[i]->m_waveShapeModel->loadSettings( _this, "wavetype" +
									is );
		m_osc[i]->m_modulationAlgoModel->loadSettings( _this,
					"modalgo" + QString::number( i+1 ) );
		m_osc[i]->m_useWaveTableModel->loadSettings( _this,
							"useWaveTable" + QString::number (i+1 ) );
		m_osc[i]->m_sampleBuffer->setAudioFile( _this.attribute(
							"userwavefile" + is ) );
	}
}




QString TripleOscillator::nodeName() const
{
	return( tripleoscillator_plugin_descriptor.name );
}




void TripleOscillator::playNote( INotePlayHandle * _n,
						sampleFrame * _working_buffer )
{
	if( _n->totalFramesPlayed() == 0 || _n->pluginData() == nullptr )
	{
		auto oscs_l = std::array<IOscillator*, NUM_OF_OSCILLATORS>{};
		auto oscs_r = std::array<IOscillator*, NUM_OF_OSCILLATORS>{};

		for( int i = NUM_OF_OSCILLATORS - 1; i >= 0; --i )
		{

			// the last oscs needs no sub-oscs...
			if( i == NUM_OF_OSCILLATORS - 1 )
			{
				oscs_l[i] = createOscillator(
						m_osc[i]->m_waveShapeModel,
						m_osc[i]->m_modulationAlgoModel,
						_n->frequency(),
						m_osc[i]->m_detuningLeft,
						m_osc[i]->m_phaseOffsetLeft,
						m_osc[i]->m_volumeLeft );
				oscs_l[i]->setUseWaveTable(m_osc[i]->m_useWaveTable);
				oscs_r[i] = createOscillator(
						m_osc[i]->m_waveShapeModel,
						m_osc[i]->m_modulationAlgoModel,
						_n->frequency(),
						m_osc[i]->m_detuningRight,
						m_osc[i]->m_phaseOffsetRight,
						m_osc[i]->m_volumeRight );
				oscs_r[i]->setUseWaveTable(m_osc[i]->m_useWaveTable);
			}
			else
			{
				oscs_l[i] = createOscillator(
						m_osc[i]->m_waveShapeModel,
						m_osc[i]->m_modulationAlgoModel,
						_n->frequency(),
						m_osc[i]->m_detuningLeft,
						m_osc[i]->m_phaseOffsetLeft,
						m_osc[i]->m_volumeLeft,
						oscs_l[i + 1]);
				oscs_l[i]->setUseWaveTable(m_osc[i]->m_useWaveTable);
				oscs_r[i] = createOscillator(
						m_osc[i]->m_waveShapeModel,
						m_osc[i]->m_modulationAlgoModel,
						_n->frequency(),
						m_osc[i]->m_detuningRight,
						m_osc[i]->m_phaseOffsetRight,
						m_osc[i]->m_volumeRight,
						oscs_r[i + 1]);
				oscs_r[i]->setUseWaveTable(m_osc[i]->m_useWaveTable);
			}

			oscs_l[i]->setUserWave( m_osc[i]->m_sampleBuffer.get() );
			oscs_r[i]->setUserWave( m_osc[i]->m_sampleBuffer.get() );

		}

		_n->setPluginData(new oscPtr);
		static_cast<oscPtr *>( _n->pluginData() )->oscLeft = oscs_l[0];
		static_cast< oscPtr *>( _n->pluginData() )->oscRight =
								oscs_r[0];
	}

	IOscillator * osc_l = static_cast<oscPtr *>( _n->pluginData() )->oscLeft;
	IOscillator * osc_r = static_cast<oscPtr *>( _n->pluginData() )->oscRight;

	const fpp_t frames = _n->framesLeftForCurrentPeriod();
	const f_cnt_t offset = _n->noteOffset();

	osc_l->update( _working_buffer + offset, frames, 0 );
	osc_r->update( _working_buffer + offset, frames, 1 );

	applyFadeIn(_working_buffer, _n);
	applyRelease( _working_buffer, _n );

	instrumentTrack()->processAudioBuffer( _working_buffer, frames + offset, _n );
}




void TripleOscillator::deleteNotePluginData( INotePlayHandle * _n )
{
	delete static_cast<IOscillator *>( static_cast<oscPtr *>(
						_n->pluginData() )->oscLeft );
	delete static_cast<IOscillator *>( static_cast<oscPtr *>(
						_n->pluginData() )->oscRight );
	delete static_cast<oscPtr *>( _n->pluginData() );
}




void TripleOscillator::updateAllDetuning()
{
	for (const auto& osc : m_osc)
	{
		osc->updateDetuningLeft();
		osc->updateDetuningRight();
	}
}



} // namespace lmms
