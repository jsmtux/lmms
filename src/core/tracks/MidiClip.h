/*
 * MidiClip.h - declaration of class MidiClip, which contains all information
 *              about a clip
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

#ifndef LMMS_MIDI_CLIP_H
#define LMMS_MIDI_CLIP_H

#include "Clip.h"
#include "Note.h"
#include "InstrumentTrack.h"


namespace lmms
{


class InstrumentTrack;

namespace gui
{
class MidiClipView;
}


class LMMS_EXPORT MidiClip : public TypedClip<InstrumentTrack>, public IMidiClip
{
	Q_OBJECT
public:
	MidiClip( InstrumentTrack* instrumentTrack );
	MidiClip( const MidiClip& other );
	~MidiClip() override;

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

	void init();

	void updateLength() override;

	// note management
	Note * addNote( const Note & _new_note, const bool _quant_pos = true ) override;

	void removeNote( Note * _note_to_del ) override;

	Note * noteAtStep( int _step ) override;

	void rearrangeAllNotes() override;
	void clearNotes();

	inline const NoteVector & notes() const override
	{
		return m_notes;
	}

	Note * addStepNote( int step ) override;
	void setStep( int step, bool enabled ) override;

	// Split the list of notes on the given position
	void splitNotes(NoteVector notes, TimePos pos) override;

	// clip-type stuff
	inline MidiClipTypes type() const override
	{
		return m_clipType;
	}


	// next/previous track based on position in the containing track
	IMidiClip * previousMidiClip() const override;
	IMidiClip * nextMidiClip() const override;

	// settings-management
	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	inline QString nodeName() const override
	{
		return "midiclip";
	}

	bool empty() override;

	ClipType getType() override
	{
		return ClipType::Midi;
	}

	int steps() override {
		return m_steps;
	}

	IInstrumentTrack* getInstrumentTrack() override {
		return getTrack();
	}

	MidiClipModel* midiClipModel() override {
		return &m_midiClipModel;
	}

public slots:
	void addSteps() override;
	void cloneSteps() override;
	void removeSteps() override;
	void clear() override;

protected:
	void updatePatternTrack();

protected slots:
	void changeTimeSignature();


private:
	MidiClipModel m_midiClipModel;
	TimePos beatClipLength() const;

	void setType( MidiClipTypes _new_clip_type );
	void checkType();

	void resizeToFirstTrack();

	MidiClipTypes m_clipType;

	// data-stuff
	NoteVector m_notes;
	int m_steps;

	MidiClip * adjacentMidiClipByOffset(int offset) const;
} ;


} // namespace lmms

#endif // LMMS_MIDI_CLIP_H
