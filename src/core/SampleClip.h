/*
 * SampleClip.h
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

#ifndef LMMS_SAMPLE_CLIP_H
#define LMMS_SAMPLE_CLIP_H

#include "Clip.h"
#include "SampleBuffer.h"

namespace lmms
{

class SampleBuffer;
class SampleTrack;

namespace gui
{

class SampleClipView;

} // namespace gui


class SampleClip : public TypedClip<SampleTrack>, public ISampleClip
{
	Q_OBJECT
	mapPropertyFromModel(bool,isRecord,setRecord,m_recordModel);
public:
	SampleClip( SampleTrack * _track );
	SampleClip( const SampleClip& orig );
	~SampleClip() override;

	IClip* baseClip() override
	{
		return this;
	}

	const IClip* baseClip() const override
	{
		return this;
	}

	virtual ClipTypeSpecificInterface* getClipTypeSpecificInterface() override {
		return this;
	}

	SampleClipModel& sampleClipModel() override {
		return m_sampleClipModel;
	}
	SampleClip& operator=( const SampleClip& that ) = delete;

	void changeLength( const TimePos & _length ) override;
	const QString & sampleFile() const;

	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;
	inline QString nodeName() const override
	{
		return "sampleclip";
	}

	ISampleBuffer* iSampleBuffer() override
	{
		return m_sampleBuffer;
	}

	SampleBuffer* sampleBuffer()
	{
		return m_sampleBuffer;
	}

	TimePos sampleLength() const override;
	void setSampleStartFrame( f_cnt_t startFrame );
	void setSamplePlayLength( f_cnt_t length );

	ClipType getType() override
	{
		return ClipType::Sample;
	}

	bool isPlaying() const;
	void setIsPlaying(bool isPlaying);

public slots:
	void setSampleBuffer( lmms::SampleBuffer* sb );
	void setSampleFile( const QString & _sf ) override;
	void updateLength() override;
	void toggleRecord() override;
	void playbackPositionChanged() override;
	void updateTrackClips() override;


private:
	SampleBuffer* m_sampleBuffer;
	BoolModel m_recordModel;
	bool m_isPlaying;
	SampleClipModel m_sampleClipModel;
} ;


} // namespace lmms

#endif // LMMS_SAMPLE_CLIP_H
