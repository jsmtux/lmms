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

#ifndef LMMS_I_MIDI_EVENT_H
#define LMMS_I_MIDI_EVENT_H

#include "MidiConstants.h"

#include <memory>

namespace lmms
{

class IMidiEvent {
public:
	virtual ~IMidiEvent() = default;
	enum class Source { Internal, External };
    virtual void setMetaEvent( MidiMetaEventType metaEvent ) = 0;
};

std::unique_ptr<IMidiEvent> createMidiEvent(MidiEventTypes type = MidiActiveSensing,
				int8_t channel = 0,
				int16_t param1 = 0,
				int16_t param2 = 0,
				const void* sourcePort = nullptr,
				IMidiEvent::Source source = IMidiEvent::Source::External);

} // namespace lmms

#endif // LMMS_I_MIDI_EVENT_H
