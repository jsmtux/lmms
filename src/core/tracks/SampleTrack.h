/*
 * SampleTrack.h - class SampleTrack, a track which provides arrangement of samples
 *
 * Copyright (c) 2005-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_SAMPLE_TRACK_H
#define LMMS_SAMPLE_TRACK_H

#include "TypedTrack.h"
#include "audio/AudioPort.h"

namespace lmms
{

namespace gui
{

class SampleTrackView;
class SampleTrackWindow;

} // namespace gui

class SampleClip;

class SampleTrack : public TypedTrack<SampleTrack>, public ISampleTrack
{
	Q_OBJECT
public:
	SampleTrack( TrackContainer* tc );
	~SampleTrack() override;

	ITrack* baseTrack() override {
		return this;
	}

	bool play( const TimePos & _start, const fpp_t _frames,
						const f_cnt_t _frame_base, int _clip_num = -1 ) override;
	IClip* createClip(const TimePos & pos) override;
	ISampleClip* createSampleClip(const TimePos & pos) override {
		return dynamic_cast<ISampleClip*>(createClip(pos));
	}
	virtual void createSampleClip(const TimePos & pos, const QString & sampleFile ) override;


	void saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _parent ) override;
	void loadTrackSpecificSettings( const QDomElement & _this ) override;

	inline IIntAutomatableModel * mixerChannelModel() override
	{
		return &m_mixerChannelModel;
	}

	inline AudioPort * audioPort()
	{
		return &m_audioPort;
	}

	QString nodeName() const override
	{
		return "sampletrack";
	}

	bool isPlaying() override
	{
		return m_isPlaying;
	}

	void setPlaying(bool playing)
	{
		if (m_isPlaying != playing) { emit m_signals.playingChanged(); }
		m_isPlaying = playing;
	}

	IFloatAutomatableModel* volumeModel() override {
		return &m_volumeModel;
	}

	IFloatAutomatableModel* panningModel() override {
		return &m_panningModel;
	}

	IAudioPort* audioPortInterface() override {
		return &m_audioPort;
	}

	SampleTrackSignals* sampleTrackSignals() override {
		return &m_signals;
	}

public slots:
	void updateClips();
	void setPlayingClips( bool isPlaying );
	void updateMixerChannel();

private:
	SampleTrackSignals m_signals;
	FloatModel m_volumeModel;
	FloatModel m_panningModel;
	IntModel m_mixerChannelModel;
	AudioPort m_audioPort;
	bool m_isPlaying;
} ;


} // namespace lmms

#endif // LMMS_SAMPLE_TRACK_H
