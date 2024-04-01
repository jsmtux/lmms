/*
 * AudioSdl.h - device-class that performs PCM-output via SDL
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_AUDIO_SDL_H
#define LMMS_AUDIO_SDL_H

#include "lmmsconfig.h"

#include <QString>

#ifdef LMMS_HAVE_SDL

#ifdef LMMS_HAVE_SDL2
#include <SDL2/SDL_audio.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#endif

#include "AudioDevice.h"

namespace lmms
{

class AudioSdl : public AudioDevice
{
public:
	AudioSdl( bool & _success_ful, AudioEngine* audioEngine );
	~AudioSdl() override;

	inline static QString name()
	{
		return AudioSdlName();
	}

private:
	void startProcessing() override;
	void stopProcessing() override;
	void applyQualitySettings() override;

	static void sdlAudioCallback( void * _udata, Uint8 * _buf, int _len );
	void sdlAudioCallback( Uint8 * _buf, int _len );

#ifdef LMMS_HAVE_SDL2
	static void sdlInputAudioCallback( void * _udata, Uint8 * _buf, int _len );
	void sdlInputAudioCallback( Uint8 * _buf, int _len );
#endif

	SDL_AudioSpec m_audioHandle;

	surroundSampleFrame * m_outBuf;

#ifdef LMMS_HAVE_SDL2
	size_t m_currentBufferFramePos;
	size_t m_currentBufferFramesCount;
#else
	Uint8 * m_convertedBuf;
	int m_convertedBufPos;
	int m_convertedBufSize;
	bool m_outConvertEndian;
#endif


	bool m_stopped;

#ifdef LMMS_HAVE_SDL2
	SDL_AudioDeviceID m_outputDevice;

	SDL_AudioSpec m_inputAudioHandle;
	SDL_AudioDeviceID m_inputDevice;
#endif

} ;


} // namespace lmms

#endif // LMMS_HAVE_SDL

#endif // LMMS_AUDIO_SDL_H