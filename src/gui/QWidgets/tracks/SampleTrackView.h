/*
 * SampleTrackView.h
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

#ifndef LMMS_GUI_SAMPLE_TRACK_VIEW_H
#define LMMS_GUI_SAMPLE_TRACK_VIEW_H


#include "TrackView.h"

namespace lmms
{

class ISampleTrack;


namespace gui
{

class Knob;
class SampleTrackWindow;
class TrackLabelButton;


class SampleTrackView : public TrackView
{
	Q_OBJECT
public:
	SampleTrackView( ISampleTrack* Track, TrackContainerView* tcv );
	~SampleTrackView() override;

	SampleTrackWindow * getSampleTrackWindow()
	{
		return m_window;
	}

	ISampleTrack * model()
	{
		return m_sampleTrack;
	}

	const ISampleTrack * model() const
	{
		return m_sampleTrack;
	}


	QMenu * createMixerMenu( QString title, QString newMixerLabel ) override;


public slots:
	void showEffects();
	void updateIndicator();


protected:
	QString nodeName() const override
	{
		return "SampleTrackView";
	}

	void dragEnterEvent(QDragEnterEvent *dee) override;
	void dropEvent(QDropEvent *de) override;

private slots:
	void assignMixerLine( int channelIndex );
	void createMixerLine();


private:
	ISampleTrack* m_sampleTrack;
	SampleTrackWindow * m_window;
	Knob * m_volumeKnob;
	Knob * m_panningKnob;
	FadeButton * m_activityIndicator;

	TrackLabelButton * m_tlb;

	FadeButton * getActivityIndicator() override
	{
		return m_activityIndicator;
	}

	friend class SampleTrackWindow;
} ;


} // namespace gui

} // namespace lmms

#endif // LMMS_GUI_SAMPLE_TRACK_VIEW_H