/*
 * AudioDeviceSetupWidget.cpp - Base class for audio device setup widgets
 *
 * Copyright (c) 2004-2015 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "AudioDeviceSetupWidget.h"

namespace lmms::gui
{

AudioDeviceSetupWidget::AudioDeviceSetupWidget(const QString & caption, QWidget * parent) :
	TabWidget(TabWidget::tr("Settings for %1").arg(tr(caption.toUtf8())), parent)
{
}

void AudioDeviceSetupWidget::show()
{
	parentWidget()->show();
	QWidget::show();
}


bool AudioDeviceSetupWidget::isAudioDevNameValid(QString name)
{
// #ifdef LMMS_HAVE_SDL
// 	if (name == AudioSdl::name())
// 	{
// 		return true;
// 	}
// #endif


// #ifdef LMMS_HAVE_ALSA
// 	if (name == AudioAlsa::name())
// 	{
// 		return true;
// 	}
// #endif


// #ifdef LMMS_HAVE_PULSEAUDIO
// 	if (name == AudioPulseAudio::name())
// 	{
// 		return true;
// 	}
// #endif


// #ifdef LMMS_HAVE_OSS
// 	if (name == AudioOss::name())
// 	{
// 		return true;
// 	}
// #endif

// #ifdef LMMS_HAVE_SNDIO
// 	if (name == AudioSndio::name())
// 	{
// 		return true;
// 	}
// #endif

// #ifdef LMMS_HAVE_JACK
// 	if (name == AudioJack::name())
// 	{
// 		return true;
// 	}
// #endif


// #ifdef LMMS_HAVE_PORTAUDIO
// 	if (name == AudioPortAudio::name())
// 	{
// 		return true;
// 	}
// #endif


// #ifdef LMMS_HAVE_SOUNDIO
// 	if (name == AudioSoundIo::name())
// 	{
// 		return true;
// 	}
// #endif

// 	if (name == AudioDummy::name())
// 	{
// 		return true;
// 	}

	return false;
}

} // namespace lmms::gui