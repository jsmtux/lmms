/*
 * TrackContainer.cpp - implementation of base class for all trackcontainers
 *                      like Song-Editor, Pattern Editor...
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


#include <QCoreApplication>
#include <QDomElement>
#include <QWriteLocker>

#include <iostream>
#include "AutomationClip.h"
#include "embed.h"
#include "TrackContainer.h"
#include "PatternClip.h"
#include "PatternStore.h"
#include "PatternTrack.h"
#include "Song.h"

#include "IGuiApplication.h"


namespace lmms
{


TrackContainer::TrackContainer(TrackContainerCb* _trackContainerCb) :
	Model( nullptr ),
	JournallingObject(),
	m_tracksMutex(),
	m_tracks(),
	m_trackContainerCb(_trackContainerCb)
{
}




TrackContainer::~TrackContainer()
{
	clearAllTracks();
}




void TrackContainer::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	_this.setTagName( classNodeName() );
	_this.setAttribute( "type", nodeName() );

	// save settings of each track
	m_tracksMutex.lockForRead();
	for (const auto& track : m_tracks)
	{
		track->saveState(_doc, _this);
	}
	m_tracksMutex.unlock();
}




void TrackContainer::loadSettings( const QDomElement & _this )
{
	bool journalRestore = _this.parentNode().nodeName() == "journaldata";
	if( journalRestore )
	{
		clearAllTracks();
	}

	static gui::IProgressModal * pd = nullptr;
	bool was_null = ( pd == nullptr );
	if (!journalRestore && gui::getGUIInterface() != nullptr)
	{
		if( pd == nullptr )
		{
			pd = gui::getGUIInterface()->mainWindowInterface()->ShowProgressMessage(
				tr( "Loading project..." ),
				0,
				Engine::getSong()->getLoadingTrackCount()
			);
		}
	}

	QDomNode node = _this.firstChild();
	while( !node.isNull() )
	{
		if( pd != nullptr )
		{
			pd->setValue( pd->value() + 1 );
			QCoreApplication::instance()->processEvents(
						QEventLoop::AllEvents, 100 );
			if( pd->wasCanceled() )
			{
				if (gui::getGUIInterface() != nullptr)
				{
					gui::getGUIInterface()->mainWindowInterface()->ShowTextFloatMessage(
						tr( "Loading cancelled" ),
						tr( "Project loading was cancelled." ),
						embed::getIconPixmap( "project_file", 24, 24 ),
						2000 );
				}
				Engine::getSong()->loadingCancelled();
				break;
			}
		}

		if( node.isElement() &&
			!node.toElement().attribute( "metadata" ).toInt() )
		{
			QString trackName = node.toElement().hasAttribute( "name" ) ?
						node.toElement().attribute( "name" ) :
						node.firstChild().toElement().attribute( "name" );
			if( pd != nullptr )
			{
				pd->updateDescription( tr("Loading Track %1 (%2/Total %3)").arg( trackName ).
						  arg( pd->value() + 1 ).arg( Engine::getSong()->getLoadingTrackCount() ) );
			}
			Track::create( node.toElement(), this );
		}
		node = node.nextSibling();
	}

	if( pd != nullptr )
	{
		if( was_null )
		{
			delete pd;
			pd = nullptr;
		}
	}
}


bool TrackContainer::allowAutoResizeClip()
{
	return m_trackContainerCb == Engine::patternStore();
}

int TrackContainer::countTracks( Track::TrackTypes _tt ) const
{
	int cnt = 0;
	m_tracksMutex.lockForRead();
	for (const auto& track : m_tracks)
	{
		if (track->type() == _tt || _tt == Track::NumTrackTypes)
		{
			++cnt;
		}
	}
	m_tracksMutex.unlock();
	return( cnt );
}




void TrackContainer::addTrack( Track * _track )
{
	if( _track->type() != Track::HiddenAutomationTrack )
	{
		_track->lock();
		m_tracksMutex.lockForWrite();
		m_tracks.push_back( _track );
		m_tracksMutex.unlock();
		_track->unlock();
		emit trackAdded( _track );
	}
}




void TrackContainer::removeTrack( Track * _track )
{
	// need a read locker to ensure that m_tracks doesn't change after reading index.
	//   After checking that index != -1, we need to upgrade the lock to a write locker before changing m_tracks.
	//   But since Qt offers no function to promote a read lock to a write lock, we must start with the write locker.
	QWriteLocker lockTracksAccess(&m_tracksMutex);
	int index = m_tracks.indexOf( _track );
	if( index != -1 )
	{
		// If the track is solo, all other tracks are muted. Change this before removing the solo track:
		if (_track->isSolo()) {
			_track->setSolo(false);
		}
		m_tracks.remove( index );
		lockTracksAccess.unlock();

		if( Engine::getSong() )
		{
			Engine::getSong()->setModified();
		}
	}
}



void TrackContainer::clearAllTracks()
{
	//m_tracksMutex.lockForWrite();
	while( !m_tracks.isEmpty() )
	{
		delete m_tracks.first();
	}
	//m_tracksMutex.unlock();
}




bool TrackContainer::isEmpty() const
{
	for (const auto& track : m_tracks)
	{
		if (!track->getClips().isEmpty())
		{
			return false;
		}
	}
	return true;
}

AutomatedValueMap TrackContainer::automatedValuesFromAllTracks(TimePos time, int clipNum, Track* addTrack) const
{
	TrackList trackList = tracks();
	if (addTrack) {
		trackList = TrackList{addTrack} << tracks();
	}
	Track::clipVector clips;

	for (Track* track: trackList)
	{
		if (track->isMuted()) {
			continue;
		}

		switch(track->type())
		{
		case Track::AutomationTrack:
		case Track::HiddenAutomationTrack:
		case Track::PatternTrack:
			if (clipNum < 0) {
				track->getClipsInRange(clips, 0, time);
			} else {
				Q_ASSERT(track->numOfClips() > clipNum);
				clips << track->getClip(clipNum);
			}
		default:
			break;
		}
	}

	AutomatedValueMap valueMap;

	Q_ASSERT(std::is_sorted(clips.begin(), clips.end(), Clip::comparePosition));

	for(Clip* clip : clips)
	{
		if (clip->isMuted() || clip->startPosition() > time) {
			continue;
		}

		if (auto* p = dynamic_cast<AutomationClip *>(clip))
		{
			if (! p->hasAutomation()) {
				continue;
			}
			TimePos relTime = time - p->startPosition();
			if (! p->getAutoResize()) {
				relTime = qMin(relTime, p->length());
			}
			float value = p->valueAt(relTime);

			for (AutomatableModel* model : p->objects())
			{
				valueMap[model] = value;
			}
		}
		else if (auto* pattern = dynamic_cast<PatternClip*>(clip))
		{
			auto patIndex = dynamic_cast<class PatternTrack*>(pattern->getTrack())->patternIndex();
			auto patStore = Engine::patternStore();

			TimePos patTime = time - clip->startPosition();
			patTime = std::min(patTime, clip->length());
			patTime = patTime % (patStore->lengthOfPattern(patIndex) * TimePos::ticksPerBar());

			auto patValues = patStore->automatedValuesAt(patTime, patIndex);
			for (auto it=patValues.begin(); it != patValues.end(); it++)
			{
				// override old values, pattern track with the highest index takes precedence
				valueMap[it.key()] = it.value();
			}
		}
		else
		{
			continue;
		}
	}

	return valueMap;
};


} // namespace lmms
