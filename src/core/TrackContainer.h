/*
 * TrackContainer.h - base-class for all track-containers like Song-Editor,
 *                    Pattern Editor...
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

#ifndef LMMS_TRACK_CONTAINER_H
#define LMMS_TRACK_CONTAINER_H

#include <QReadWriteLock>

#include "Track.h"
#include "JournallingObject.h"
#include "ITrackContainer.h"

namespace lmms
{

class AutomationClip;
class InstrumentTrack;

namespace gui
{

class TrackContainerView;

}

class TrackContainerCb {
public:
	virtual ~TrackContainerCb() = default;
	virtual AutomatedValueMap automatedValuesAt(TimePos time, int clipNum = -1) const = 0;
	virtual QString nodeName() const = 0;
};

class LMMS_EXPORT TrackContainer : public ITrackContainer
{
	Q_OBJECT
public:
	TrackContainer(TrackContainerCb* _trackContainerCb);
	~TrackContainer() override;

	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;

	void loadSettings( const QDomElement & _this ) override;

	int countTracks( Track::TrackTypes _tt = Track::NumTrackTypes ) const override;

	QString nodeName() const override
	{
		return m_trackContainerCb->nodeName();
	}

	void addTrack( Track * _track );
	void removeTrack( Track * _track );

	void clearAllTracks() override;

	const TrackList & tracks() const override
	{
		return m_tracks;
	}

	TrackList & tracks() override
	{
		return m_tracks;
	}

	bool isEmpty() const override;

	static const QString classNodeName()
	{
		return "trackcontainer";
	}

	bool allowAutoResizeClip();

	void insertBar(TimePos position)
	{
		m_tracksMutex.lockForRead();
		for (ITrack* track: tracks())
		{
			// FIXME journal batch of tracks instead of each track individually
			if (track->numOfClips() > 0) { track->addJournalCheckPoint(); }
			track->insertBar(position);
		}
		m_tracksMutex.unlock();
	}

	void removeBar(TimePos position)
	{
		m_tracksMutex.lockForRead();
		for (ITrack* track: tracks())
		{
			// FIXME journal batch of tracks instead of each track individually
			if (track->numOfClips() > 0) { track->addJournalCheckPoint(); }
			track->removeBar(position);
		}
		m_tracksMutex.unlock();
	}

	bar_t getLargestTrackLength(bool ignoreMuted = false) const {
		bar_t ret;
		m_tracksMutex.lockForRead();
		for (auto track : tracks())
		{
			if (ignoreMuted && track->isMuted())
			{
				continue;
			}

			const bar_t cur = track->length();
			if( cur > ret )
			{
				ret = cur;
			}
		}
		m_tracksMutex.unlock();
		return ret;
	}

	AutomatedValueMap automatedValuesAt(TimePos time, int clipNum = -1) const;

	AutomatedValueMap automatedValuesFromAllTracks(TimePos time, int clipNum = -1, Track* addTrack = nullptr) const;

protected:

	mutable QReadWriteLock m_tracksMutex;

private:
	TrackList m_tracks;

	TrackContainerCb* m_trackContainerCb;

	friend class gui::TrackContainerView;
	friend class Track;
} ;

} // namespace lmms

#endif // LMMS_TRACK_CONTAINER_H
