/*
 * Piano.h - declaration of class Piano
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

#ifndef LMMS_PIANO_H
#define LMMS_PIANO_H

#include "IPiano.h"
#include "Note.h"
#include "Model.h"
#include "lmms_constants.h"
#include "midi/MidiEventProcessor.h"

namespace lmms
{

class InstrumentTrack;
class IInstrumentTrack;
class MidiEventProcessor;

class Piano final: public IPiano
{
public:
	enum KeyTypes
	{
		WhiteKey,
		BlackKey
	} ;

	Piano(InstrumentTrack* track);

	void setKeyState(int key, bool state) override;

	bool isKeyPressed(int key) const override
	{
		return m_pressedKeys[key];
	}

	void handleKeyPress(int key, int midiVelocity = -1) override;
	void handleKeyRelease(int key) override;

	IInstrumentTrack* instrumentTrack() override;

	void processInEvent( const IMidiEvent* event, const TimePos& time, f_cnt_t offset ) override {
		return m_midiEvProc->processInEvent(static_cast<const MidiEvent&>(*event), time, offset);
	}

	MidiEventProcessor* midiEventProcessor() const
	{
		return m_midiEvProc;
	}

	Model* model() override
	{
		return &m_pianoModel;
	}

private:
	static bool isValidKey(int key)
	{
		return key >= 0 && key < NumKeys;
	}

	Model m_pianoModel;

	InstrumentTrack* m_instrumentTrack;
	MidiEventProcessor* m_midiEvProc;
	std::array<bool, NumKeys> m_pressedKeys = {};

} ;


} // namespace lmms

#endif // LMMS_PIANO_H
