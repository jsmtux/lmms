/*
 * Plugin.h - class plugin, the base-class and generic interface for all plugins
 *
 * Copyright (c) 2005-2023 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_I_TRACK_CONTAINER_H
#define LMMS_I_TRACK_CONTAINER_H

#include <QObject>

#include "ITrack.h"

namespace lmms {
class ITrackContainer : public Model, public JournallingObject
{
    Q_OBJECT
public:
    ITrackContainer() : Model(nullptr) {}
    virtual ~ITrackContainer() = default;
    virtual bool isEmpty() const = 0;
    virtual const TrackList & tracks() const = 0;
    virtual TrackList & tracks() = 0;
    virtual void clearAllTracks() = 0;
    virtual int countTracks( ITrack::TrackTypes _tt = ITrack::NumTrackTypes) const = 0;

signals:
	void trackAdded( lmms::ITrack * _track );
};

} //namespace lmms

#endif // LMMS_I_TRACK_CONTAINER_H