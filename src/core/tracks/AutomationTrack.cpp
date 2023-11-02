/*
 * AutomationTrack.cpp - AutomationTrack handles automation of objects without
 *                       a track
 *
 * Copyright (c) 2008-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2006-2008 Javier Serrano Polo <jasp00/at/users.sourceforge.net>
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

#include "AutomationTrack.h"

#include "AutomationClip.h"
#include "TrackContainer.h"

namespace lmms
{


IAutomationTrack* createAutomationTrack(ITrackContainer* tc) {
	return new AutomationTrack(dynamic_cast<TrackContainer*>(tc));
}

AutomationTrack::AutomationTrack( TrackContainer* tc, bool _hidden ) :
	TypedTrack( _hidden ? HiddenAutomationTrack : Track::AutomationTrack, tc )
{
	setName( tr( "Automation track" ) );
}

bool AutomationTrack::play( const TimePos & time_start, const fpp_t _frames,
							const f_cnt_t _frame_base, int _clip_num )
{
	return false;
}


IClip* AutomationTrack::createClip(const TimePos & pos)
{
	return createAutomationClip(pos)->baseClip();
}

IAutomationClip* AutomationTrack::createAutomationClip(TimePos pos)
{
	auto p = new AutomationClip(this);
	p->movePosition(pos);
	return p;
}



void AutomationTrack::saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _this )
{
}




void AutomationTrack::loadTrackSpecificSettings( const QDomElement & _this )
{
	// just in case something somehow wrent wrong...
	if( type() == HiddenAutomationTrack )
	{
		setMuted( false );
	}
}

} // namespace lmms