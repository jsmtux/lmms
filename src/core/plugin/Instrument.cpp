/*
 * Instrument.cpp - base-class for all instrument-plugins (synths, samplers etc)
 *
 * Copyright (c) 2005-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "Instrument.h"

#include <cmath>

#include "IAudioEngine.h"
#include "IEngine.h"
#include "lmms_constants.h"


namespace lmms
{

IInstrument* InstantiateInstrument(const QString& pluginName,
				ITrack * _parent,
				const PluginDescriptor::Key * _key,
                bool keyFromDnd) {
	return Instrument::instantiate(pluginName, _parent, _key, keyFromDnd);
}

Instrument::Instrument(ITrack * _instrument_track,
			const PluginDescriptor * _descriptor,
			const PluginDescriptor::Key *key) :
	Plugin(_descriptor, key),
	m_instrumentTrack( _instrument_track )
{
}

void Instrument::play( sampleFrame * )
{
}




void Instrument::deleteNotePluginData( INotePlayHandle * )
{
}




f_cnt_t Instrument::beatLen( INotePlayHandle * ) const
{
	return( 0 );
}




IInstrument *Instrument::instantiate(const QString &_plugin_name,
	ITrack *_instrument_track, const PluginDescriptor::Key *key, bool keyFromDnd)
{
	if(keyFromDnd)
		Q_ASSERT(!key);
	// copy from above // TODO! common cleaner func
	Plugin * p = Plugin::instantiateWithKey(_plugin_name, _instrument_track->model(), key, keyFromDnd);
	if(dynamic_cast<Instrument *>(p))
		return dynamic_cast<Instrument *>(p);
	delete p;
	return( InstantiateDummyInstrument( _instrument_track ) );
}




bool Instrument::isFromTrack( const ITrack * _track ) const
{
	return( m_instrumentTrack == _track );
}

// helper function for Instrument::applyFadeIn
static int countZeroCrossings(sampleFrame *buf, fpp_t start, fpp_t frames)
{
	// zero point crossing counts of all channels
	auto zeroCrossings = std::array<int, DEFAULT_CHANNELS>{};
	// maximum zero point crossing of all channels
	int maxZeroCrossings = 0;

	// determine the zero point crossing counts
	for (fpp_t f = start; f < frames; ++f)
	{
		for (ch_cnt_t ch = 0; ch < DEFAULT_CHANNELS; ++ch)
		{
			// we don't want to count [-1, 0, 1] as two crossings
			if ((buf[f - 1][ch] <= 0.0 && buf[f][ch] > 0.0) ||
					(buf[f - 1][ch] >= 0.0 && buf[f][ch] < 0.0))
			{
				++zeroCrossings[ch];
				if (zeroCrossings[ch] > maxZeroCrossings)
				{
					maxZeroCrossings = zeroCrossings[ch];
				}
			}
		}
	}

	return maxZeroCrossings;
}

// helper function for Instrument::applyFadeIn
fpp_t getFadeInLength(float maxLength, fpp_t frames, int zeroCrossings)
{
	// calculate the length of the fade in
	// Length is inversely proportional to the max of zeroCrossings,
	// because for low frequencies, we need a longer fade in to
	// prevent clicking.
	return (fpp_t) (maxLength  / ((float) zeroCrossings / ((float) frames / 128.0f) + 1.0f));
}


void Instrument::applyFadeIn(sampleFrame * buf, INotePlayHandle * n)
{
	const static float MAX_FADE_IN_LENGTH = 85.0;
	f_cnt_t total = n->totalFramesPlayed();
	if (total == 0)
	{
		const fpp_t frames = n->framesLeftForCurrentPeriod();
		const f_cnt_t offset = n->offset();

		// We need to skip the first sample because it almost always
		// produces a zero crossing; it's not helpful while
		// determining the fade in length. Hence 1
		int maxZeroCrossings = countZeroCrossings(buf, offset + 1, offset + frames);

		fpp_t length = getFadeInLength(MAX_FADE_IN_LENGTH, frames, maxZeroCrossings);
		n->setFadeInLength(length);

		// apply fade in
		length = length < frames ? length : frames;
		for (fpp_t f = 0; f < length; ++f)
		{
			for (ch_cnt_t ch = 0; ch < DEFAULT_CHANNELS; ++ch)
			{
				buf[offset + f][ch] *= 0.5 - 0.5 * cosf(F_PI * (float) f / (float) n->fadeInLength());
			}
		}
	}
	else if (total < n->fadeInLength())
	{
		const fpp_t frames = n->framesLeftForCurrentPeriod();

		int new_zc = countZeroCrossings(buf, 1, frames);
		fpp_t new_length = getFadeInLength(MAX_FADE_IN_LENGTH, frames, new_zc);

		for (fpp_t f = 0; f < frames; ++f)
		{
			for (ch_cnt_t ch = 0; ch < DEFAULT_CHANNELS; ++ch)
			{
				float currentLength = n->fadeInLength() * (1.0f - (float) f / frames) + new_length * ((float) f / frames);
				buf[f][ch] *= 0.5 - 0.5 * cosf(F_PI * (float) (total + f) / currentLength);
				if (total + f >= currentLength)
				{
					n->setFadeInLength(currentLength);
					return;
				}
			}
		}
		n->setFadeInLength(new_length);
	}
}

void Instrument::applyRelease( sampleFrame * buf, const INotePlayHandle * _n )
{
	const fpp_t frames = _n->framesLeftForCurrentPeriod();
	const fpp_t fpp = IEngine::Instance()->getAudioEngineInterface()->framesPerPeriod();
	const f_cnt_t fl = _n->framesLeft();
	if( fl <= desiredReleaseFrames()+fpp )
	{
		for( fpp_t f = (fpp_t)( ( fl > desiredReleaseFrames() ) ?
				( qMax( fpp - desiredReleaseFrames(), 0 ) +
					fl % fpp ) : 0 ); f < frames; ++f )
		{
			const float fac = (float)( fl-f-1 ) /
							desiredReleaseFrames();
			for( ch_cnt_t ch = 0; ch < DEFAULT_CHANNELS; ++ch )
			{
				buf[f][ch] *= fac;
			}
		}
	}
}




// QString Instrument::fullDisplayName() const
// {
// 	return iTrack()->displayName();
// }


} // namespace lmms
