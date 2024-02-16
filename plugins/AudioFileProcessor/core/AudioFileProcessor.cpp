/*
 * AudioFileProcessor.cpp - instrument for using audio-files
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

#include "AudioFileProcessor.h"

#include <QFileInfo>
#include <samplerate.h>

#include "IAudioEngine.h"
#include "IEngine.h"
#include "ITrack.h"
#include "IPlayHandle.h"
#include "IPathUtil.h"

#include "embed.h"


namespace lmms
{


extern "C"
{

PluginDescriptor audiofileprocessor_plugin_descriptor =
{
	LMMS_STRINGIFY( PLUGIN_NAME ),
	"AudioFileProcessor",
	QT_TRANSLATE_NOOP( "PluginBrowser",
				"Simple sampler with various settings for "
				"using samples (e.g. drums) in an "
				"instrument-track" ),
	"Tobias Doerffel <tobydox/at/users.sf.net>",
	0x0100,
	PluginTypes::Instrument,
	nullptr,
	nullptr,
	// "wav,ogg,ds,spx,au,voc,aif,aiff,flac,raw",
	nullptr,
} ;

}




AudioFileProcessor::AudioFileProcessor( IInstrumentTrack * _instrument_track ) :
	Instrument( _instrument_track, &audiofileprocessor_plugin_descriptor ),
	m_sampleBuffer(createSampleBuffer()),
	m_ampModel( MFact::create(100.0f, 0.0f, 500.0f, 1.0f, this, tr( "Amplify" ) )),
	m_startPointModel( MFact::create(0.0f, 0.0f, 1.0f, 0.0000001f, this, tr( "Start of sample" ) )),
	m_endPointModel( MFact::create(1.0f, 0.0f, 1.0f, 0.0000001f, this, tr( "End of sample" ) )),
	m_loopPointModel( MFact::create(0.0f, 0.0f, 1.0f, 0.0000001f, this, tr( "Loopback point" ) )),
	m_reverseModel( MFact::create(false, this, tr( "Reverse sample" ) )),
	m_loopModel( MFact::createIntModel(0, 0, 2, this, tr( "Loop mode" ) )),
	m_stutterModel( MFact::create(false, this, tr( "Stutter" ) )),
	m_interpolationModel( MFact::createComboBox(this, tr( "Interpolation mode" ) )),
	m_nextPlayStartPoint( 0 ),
	m_nextPlayBackwards( false )
{
	connect( m_reverseModel->model(), &Model::dataChanged,
				this, &AudioFileProcessor::reverseModelChanged, Qt::DirectConnection );
	connect( m_ampModel->model(), &Model::dataChanged,
				this, &AudioFileProcessor::ampModelChanged, Qt::DirectConnection );
	connect( m_startPointModel->model(), &Model::dataChanged,
				this, &AudioFileProcessor::startPointChanged, Qt::DirectConnection );
	connect( m_endPointModel->model(), &Model::dataChanged,
				this, &AudioFileProcessor::endPointChanged, Qt::DirectConnection );
	connect( m_loopPointModel->model(), &Model::dataChanged,
				this, &AudioFileProcessor::loopPointChanged, Qt::DirectConnection );
	connect( m_stutterModel->model(), &Model::dataChanged,
				this, &AudioFileProcessor::stutterModelChanged, Qt::DirectConnection );

//interpolation modes
	m_interpolationModel->addItem( tr( "None" ) );
	m_interpolationModel->addItem( tr( "Linear" ) );
	m_interpolationModel->addItem( tr( "Sinc" ) );
	m_interpolationModel->setValue( 1 );

	pointChanged();
}




void AudioFileProcessor::playNote( INotePlayHandle * _n,
						sampleFrame * _working_buffer )
{
	const fpp_t frames = _n->framesLeftForCurrentPeriod();
	const f_cnt_t offset = _n->noteOffset();

	// Magic key - a frequency < 20 (say, the bottom piano note if using
	// a A4 base tuning) restarts the start point. The note is not actually
	// played.
	if( m_stutterModel->value() == true && _n->frequency() < 20.0 )
	{
		m_nextPlayStartPoint = m_sampleBuffer->startFrame();
		m_nextPlayBackwards = false;
		return;
	}

	if( !_n->pluginData() )
	{
		if( m_stutterModel->value() == true && m_nextPlayStartPoint >= m_sampleBuffer->endFrame() )
		{
			// Restart playing the note if in stutter mode, not in loop mode,
			// and we're at the end of the sample.
			m_nextPlayStartPoint = m_sampleBuffer->startFrame();
			m_nextPlayBackwards = false;
		}
		// set interpolation mode for libsamplerate
		int srcmode = SRC_LINEAR;
		switch( m_interpolationModel->wrappedModel()->value() )
		{
			case 0:
				srcmode = SRC_ZERO_ORDER_HOLD;
				break;
			case 1:
				srcmode = SRC_LINEAR;
				break;
			case 2:
				srcmode = SRC_SINC_MEDIUM_QUALITY;
				break;
		}
		auto state = createHandleState(_n->hasDetuningInfo(), srcmode);
		state->setFrameIndex( m_nextPlayStartPoint );
		state->setBackwards( m_nextPlayBackwards );
		_n->setPluginData(state);


// debug code
/*		qDebug( "frames %d", m_sampleBuffer->frames() );
		qDebug( "startframe %d", m_sampleBuffer->startFrame() );
		qDebug( "nextPlayStartPoint %d", m_nextPlayStartPoint );*/
	}

	if( ! _n->isFinished() )
	{
		if( m_sampleBuffer->play( _working_buffer + offset,
						(IHandleState *)_n->pluginData(),
						frames, _n->frequency(),
						static_cast<ISampleBuffer::LoopMode>( m_loopModel->value() ) ) )
		{
			applyRelease( _working_buffer, _n );
			instrumentTrack()->processAudioBuffer( _working_buffer,
									frames + offset, _n );

			emit isPlaying( ((IHandleState *)_n->pluginData())->frameIndex() );
		}
		else
		{
			memset( _working_buffer, 0, ( frames + offset ) * sizeof( sampleFrame ) );
			emit isPlaying( 0 );
		}
	}
	else
	{
		emit isPlaying( 0 );
	}
	if( m_stutterModel->value() == true )
	{
		m_nextPlayStartPoint = ((IHandleState *)_n->pluginData())->frameIndex();
		m_nextPlayBackwards = ((IHandleState *)_n->pluginData())->isBackwards();
	}
}




void AudioFileProcessor::deleteNotePluginData( INotePlayHandle * _n )
{
	delete (IHandleState *)_n->pluginData();
}




void AudioFileProcessor::saveSettings( QDomDocument & _doc,
							QDomElement & _this )
{
	_this.setAttribute( "src", m_sampleBuffer->audioFile() );
	if( m_sampleBuffer->audioFile() == "" )
	{
		QString s;
		_this.setAttribute( "sampledata",
						m_sampleBuffer->toBase64( s ) );
	}
	m_reverseModel->saveSettings( _doc, _this, "reversed" );
	m_loopModel->saveSettings( _doc, _this, "looped" );
	m_ampModel->saveSettings( _doc, _this, "amp" );
	m_startPointModel->saveSettings( _doc, _this, "sframe" );
	m_endPointModel->saveSettings( _doc, _this, "eframe" );
	m_loopPointModel->saveSettings( _doc, _this, "lframe" );
	m_stutterModel->saveSettings( _doc, _this, "stutter" );
	m_interpolationModel->wrappedModel()->saveSettings( _doc, _this, "interp" );

}




void AudioFileProcessor::loadSettings( const QDomElement & _this )
{
	if( _this.attribute( "src" ) != "" )
	{
		setAudioFile( _this.attribute( "src" ), false );

		QString absolutePath = PathUtil::toAbsolute( m_sampleBuffer->audioFile() );
		if ( !QFileInfo( absolutePath ).exists() )
		{
			QString message = tr( "Sample not found: %1" ).arg( m_sampleBuffer->audioFile() );

			IEngine::Instance()->getSongInterface()->collectError( message );
		}
	}
	else if( _this.attribute( "sampledata" ) != "" )
	{
		m_sampleBuffer->loadFromBase64( _this.attribute( "srcdata" ) );
	}

	m_loopModel->loadSettings( _this, "looped" );
	m_ampModel->loadSettings( _this, "amp" );
	m_endPointModel->loadSettings( _this, "eframe" );
	m_startPointModel->loadSettings( _this, "sframe" );

	// compat code for not having a separate loopback point
	if (_this.hasAttribute("lframe") || !(_this.firstChildElement("lframe").isNull()))
	{
		m_loopPointModel->loadSettings( _this, "lframe" );
	}
	else
	{
		m_loopPointModel->loadSettings( _this, "sframe" );
	}

	m_reverseModel->loadSettings( _this, "reversed" );

	m_stutterModel->loadSettings( _this, "stutter" );
	if( _this.hasAttribute( "interp" ) )
	{
		m_interpolationModel->wrappedModel()->loadSettings( _this, "interp" );
	}
	else
	{
		m_interpolationModel->setValue( 1 ); //linear by default
	}

	pointChanged();
}




void AudioFileProcessor::loadFile( const QString & _file )
{
	setAudioFile( _file );
}




QString AudioFileProcessor::nodeName() const
{
	return audiofileprocessor_plugin_descriptor.name;
}




int AudioFileProcessor::getBeatLen( INotePlayHandle * _n ) const
{
	const auto baseFreq = instrumentTrack()->baseFreq();
	const float freq_factor = baseFreq / _n->frequency() *
			IEngine::Instance()->getAudioEngineInterface()->processingSampleRate() / IEngine::Instance()->getAudioEngineInterface()->baseSampleRate();

	return static_cast<int>( floorf( ( m_sampleBuffer->endFrame() - m_sampleBuffer->startFrame() ) * freq_factor ) );
}


void AudioFileProcessor::setAudioFile( const QString & _audio_file,
													bool _rename )
{
	// is current channel-name equal to previous-filename??
	if( _rename &&
		( instrumentTrack()->baseTrack()->name() ==
			QFileInfo( m_sampleBuffer->audioFile() ).fileName() ||
				m_sampleBuffer->audioFile().isEmpty() ) )
	{
		// then set it to new one
		instrumentTrack()->baseTrack()->setName( PathUtil::cleanName( _audio_file ) );
	}
	// else we don't touch the track-name, because the user named it self

	m_sampleBuffer->setAudioFile( _audio_file );
	loopPointChanged();
}




void AudioFileProcessor::reverseModelChanged()
{
	m_sampleBuffer->setReversed( m_reverseModel->value() );
	m_nextPlayStartPoint = m_sampleBuffer->startFrame();
	m_nextPlayBackwards = false;
}




void AudioFileProcessor::ampModelChanged()
{
	m_sampleBuffer->setAmplification( m_ampModel->value() / 100.0f );
}


void AudioFileProcessor::stutterModelChanged()
{
	m_nextPlayStartPoint = m_sampleBuffer->startFrame();
	m_nextPlayBackwards = false;
}


void AudioFileProcessor::startPointChanged()
{
	// check if start is over end and swap values if so
	if( m_startPointModel->value() > m_endPointModel->value() )
	{
		float tmp = m_endPointModel->value();
		m_endPointModel->setValue( m_startPointModel->value() );
		m_startPointModel->setValue( tmp );
	}

	// nudge loop point with end
	if( m_loopPointModel->value() >= m_endPointModel->value() )
	{
		m_loopPointModel->setValue( qMax( m_endPointModel->value() - 0.001f, 0.0f ) );
	}

	// nudge loop point with start
	if( m_loopPointModel->value() < m_startPointModel->value() )
	{
		m_loopPointModel->setValue( m_startPointModel->value() );
	}

	// check if start & end overlap and nudge end up if so
	if( m_startPointModel->value() == m_endPointModel->value() )
	{
		m_endPointModel->setValue( qMin( m_endPointModel->value() + 0.001f, 1.0f ) );
	}

	pointChanged();

}

void AudioFileProcessor::endPointChanged()
{
	// same as start, for now
	startPointChanged();

}

void AudioFileProcessor::loopPointChanged()
{

	// check that loop point is between start-end points and not overlapping with endpoint
	// ...and move start/end points ahead if loop point is moved over them
	if( m_loopPointModel->value() >= m_endPointModel->value() )
	{
		m_endPointModel->setValue( m_loopPointModel->value() + 0.001f );
		if( m_endPointModel->value() == 1.0f )
		{
			m_loopPointModel->setValue( 1.0f - 0.001f );
		}
	}

	// nudge start point with loop
	if( m_loopPointModel->value() < m_startPointModel->value() )
	{
		m_startPointModel->setValue( m_loopPointModel->value() );
	}

	pointChanged();
}

void AudioFileProcessor::pointChanged()
{
	const auto f_start = static_cast<f_cnt_t>(m_startPointModel->value() * m_sampleBuffer->frames());
	const auto f_end = static_cast<f_cnt_t>(m_endPointModel->value() * m_sampleBuffer->frames());
	const auto f_loop = static_cast<f_cnt_t>(m_loopPointModel->value() * m_sampleBuffer->frames());

	m_nextPlayStartPoint = f_start;
	m_nextPlayBackwards = false;

	m_sampleBuffer->setAllPointFrames( f_start, f_end, f_loop, f_end );
}


} // namespace lmms
