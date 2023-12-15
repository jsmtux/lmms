/*
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

#ifndef LMMS_TYPED_TRACK_H
#define LMMS_TYPED_TRACK_H

#include "Track.h"
#include "Clip.h"

namespace lmms
{

template<typename SpecialisedTrack>
class TypedTrack : public Track
{
public:
	TypedTrack( TrackTypes type, TrackContainer * tc ) :
		Track(type, tc)
	{}
	// -- for usage by Clip only ---------------
	void addClip( TypedClip<SpecialisedTrack> * clip )
	{
		m_clips.push_back( clip );
	}
	void removeClip( TypedClip<SpecialisedTrack> * clip )
	{
		clipVector::iterator it = std::find( m_clips.begin(), m_clips.end(), clip );
		if( it != m_clips.end() )
		{
			emit clipRemoved( *it );
			updateSongProperties();
			m_clips.erase( it );
		}
	}
};


} // namespace lmms

#endif // LMMS_TYPED_TRACK_H
