/*
 * AudioPortAudio.cpp - device-class that performs PCM-output via PortAudio
 *
 * Copyright (c) 2008 Csaba Hruska <csaba.hruska/at/gmail.com>
 * Copyright (c) 2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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


#include "AudioPortAudio.h"


#ifdef LMMS_HAVE_PORTAUDIO

#include "Engine.h"
#include "ConfigManager.h"
#include "AudioEngine.h"

namespace lmms
{


AudioPortAudio::AudioPortAudio( bool & _success_ful, AudioEngine * _audioEngine ) :
	AudioDevice( qBound<ch_cnt_t>(
		DEFAULT_CHANNELS,
		ConfigManager::inst()->value( "audioportaudio", "channels" ).toInt(),
		SURROUND_CHANNELS ), _audioEngine ),
	m_paStream( nullptr ),
	m_wasPAInitError( false ),
	m_outBuf( new surroundSampleFrame[audioEngine()->framesPerPeriod()] ),
	m_outBufPos( 0 )
{
	_success_ful = false;

	m_outBufSize = audioEngine()->framesPerPeriod();

	PaError err = Pa_Initialize();
	
	if( err != paNoError ) {
		printf( "Couldn't initialize PortAudio: %s\n", Pa_GetErrorText( err ) );
		m_wasPAInitError = true;
		return;
	}

	if( Pa_GetDeviceCount() <= 0 )
	{
		return;
	}
	
	const QString& backend = ConfigManager::inst()->value( "audioportaudio", "backend" );
	const QString& device = ConfigManager::inst()->value( "audioportaudio", "device" );
		
	PaDeviceIndex inDevIdx = -1;
	PaDeviceIndex outDevIdx = -1;
	const PaDeviceInfo * di;
	for( int i = 0; i < Pa_GetDeviceCount(); ++i )
	{
		di = Pa_GetDeviceInfo( i );
		if( di->name == device &&
			Pa_GetHostApiInfo( di->hostApi )->name == backend )
		{
			inDevIdx = i;
			outDevIdx = i;
		}
	}

	if( inDevIdx < 0 )
	{
		inDevIdx = Pa_GetDefaultInputDevice();
	}
	
	if( outDevIdx < 0 )
	{
		outDevIdx = Pa_GetDefaultOutputDevice();
	}

	if( inDevIdx < 0 || outDevIdx < 0 )
	{
		return;
	}

	double inLatency = 0;//(double)audioEngine()->framesPerPeriod() / (double)sampleRate();
	double outLatency = 0;//(double)audioEngine()->framesPerPeriod() / (double)sampleRate();

	//inLatency = Pa_GetDeviceInfo( inDevIdx )->defaultLowInputLatency;
	//outLatency = Pa_GetDeviceInfo( outDevIdx )->defaultLowOutputLatency;
	const int samples = audioEngine()->framesPerPeriod();
	
	// Configure output parameters.
	m_outputParameters.device = outDevIdx;
	m_outputParameters.channelCount = channels();
	m_outputParameters.sampleFormat = paFloat32; // 32 bit floating point output
	m_outputParameters.suggestedLatency = outLatency;
	m_outputParameters.hostApiSpecificStreamInfo = nullptr;
	
	// Configure input parameters.
	m_inputParameters.device = inDevIdx;
	m_inputParameters.channelCount = DEFAULT_CHANNELS;
	m_inputParameters.sampleFormat = paFloat32; // 32 bit floating point input
	m_inputParameters.suggestedLatency = inLatency;
	m_inputParameters.hostApiSpecificStreamInfo = nullptr;
	
	// Open an audio I/O stream. 
	err = Pa_OpenStream(
			&m_paStream,
			supportsCapture() ? &m_inputParameters : nullptr,	// The input parameter
			&m_outputParameters,	// The outputparameter
			sampleRate(),
			samples,
			paNoFlag,		// Don't use any flags
			_process_callback, 	// our callback function
			this );

	if( err == paInvalidDevice && sampleRate() < 48000 )
	{
		printf("Pa_OpenStream() failed with 44,1 KHz, trying again with 48 KHz\n");
		// some backends or drivers do not allow 32 bit floating point data
		// with a samplerate of 44100 Hz
		setSampleRate( 48000 );
		err = Pa_OpenStream(
				&m_paStream,
				supportsCapture() ? &m_inputParameters : nullptr,	// The input parameter
				&m_outputParameters,	// The outputparameter
				sampleRate(),
				samples,
				paNoFlag,		// Don't use any flags
				_process_callback, 	// our callback function
				this );
	}

	if( err != paNoError )
	{
		printf( "Couldn't open PortAudio: %s\n", Pa_GetErrorText( err ) );
		return;
	}

	printf( "Input device: '%s' backend: '%s'\n", Pa_GetDeviceInfo( inDevIdx )->name, Pa_GetHostApiInfo( Pa_GetDeviceInfo( inDevIdx )->hostApi )->name );
	printf( "Output device: '%s' backend: '%s'\n", Pa_GetDeviceInfo( outDevIdx )->name, Pa_GetHostApiInfo( Pa_GetDeviceInfo( outDevIdx )->hostApi )->name );

	// TODO: debug AudioEngine::pushInputFrames()
	//m_supportsCapture = true;

	_success_ful = true;
}




AudioPortAudio::~AudioPortAudio()
{
	stopProcessing();

	if( !m_wasPAInitError )
	{
		Pa_Terminate();
	}
	delete[] m_outBuf;
}




void AudioPortAudio::startProcessing()
{
	m_stopped = false;
	PaError err = Pa_StartStream( m_paStream );
	
	if( err != paNoError )
	{
		m_stopped = true;
		printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	}
}




void AudioPortAudio::stopProcessing()
{
	if( m_paStream && Pa_IsStreamActive( m_paStream ) )
	{
		m_stopped = true;
		PaError err = Pa_StopStream( m_paStream );
	
		if( err != paNoError )
		{
			printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
		}
	}
}




void AudioPortAudio::applyQualitySettings()
{
	if( hqAudio() )
	{

		setSampleRate( Engine::audioEngine()->processingSampleRate() );
		int samples = audioEngine()->framesPerPeriod();

		PaError err = Pa_OpenStream(
			&m_paStream,
			supportsCapture() ? &m_inputParameters : nullptr,	// The input parameter
			&m_outputParameters,	// The outputparameter
			sampleRate(),
			samples,
			paNoFlag,		// Don't use any flags
			_process_callback, 	// our callback function
			this );
	
		if( err != paNoError )
		{
			printf( "Couldn't open PortAudio: %s\n", Pa_GetErrorText( err ) );
			return;
		}
	}

	AudioDevice::applyQualitySettings();
}



int AudioPortAudio::process_callback(
	const float *_inputBuffer,
	float * _outputBuffer,
	unsigned long _framesPerBuffer )
{
	if( supportsCapture() )
	{
		audioEngine()->pushInputFrames( (sampleFrame*)_inputBuffer, _framesPerBuffer );
	}

	if( m_stopped )
	{
		memset( _outputBuffer, 0, _framesPerBuffer *
			channels() * sizeof(float) );
		return paComplete;
	}

	while( _framesPerBuffer )
	{
		if( m_outBufPos == 0 )
		{
			// frames depend on the sample rate
			const fpp_t frames = getNextBuffer( m_outBuf );
			if( !frames )
			{
				m_stopped = true;
				memset( _outputBuffer, 0, _framesPerBuffer *
					channels() * sizeof(float) );
				return paComplete;
			}
			m_outBufSize = frames;
		}
		const int min_len = qMin( (int)_framesPerBuffer,
			m_outBufSize - m_outBufPos );

		float master_gain = audioEngine()->masterGain();

		for( fpp_t frame = 0; frame < min_len; ++frame )
		{
			for( ch_cnt_t chnl = 0; chnl < channels(); ++chnl )
			{
				( _outputBuffer + frame * channels() )[chnl] =
						AudioEngine::clip( m_outBuf[frame][chnl] *
						master_gain );
			}
		}

		_outputBuffer += min_len * channels();
		_framesPerBuffer -= min_len;
		m_outBufPos += min_len;
		m_outBufPos %= m_outBufSize;
	}

	return paContinue;
}



int AudioPortAudio::_process_callback(
	const void *_inputBuffer,
	void * _outputBuffer,
	unsigned long _framesPerBuffer,
	const PaStreamCallbackTimeInfo * _timeInfo,
	PaStreamCallbackFlags _statusFlags,
	void * _arg )
{
	Q_UNUSED(_timeInfo);
	Q_UNUSED(_statusFlags);

	auto _this = static_cast<AudioPortAudio*>(_arg);
	return _this->process_callback( (const float*)_inputBuffer,
		(float*)_outputBuffer, _framesPerBuffer );
}



} // namespace lmms


#endif // LMMS_HAVE_PORTAUDIO



