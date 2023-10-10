/*
 * InstrumentMidiIOView.h - widget in instrument-track-window for setting
 *                          up MIDI-related stuff
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

#ifndef LMMS_GUI_INSTRUMENT_MIDI_IO_VIEW_H
#define LMMS_GUI_INSTRUMENT_MIDI_IO_VIEW_H

#include <QWidget>

class QToolButton;

namespace lmms
{

class InstrumentTrack;
class MidiPort;

namespace gui
{

class GroupBox;
class LcdSpinBox;
class LedCheckBox;


class InstrumentMidiIOView : public QWidget
{
	Q_OBJECT
public:
	InstrumentMidiIOView( MidiPort* mp, QWidget* parent );
	~InstrumentMidiIOView() override = default;

private:
	MidiPort* m_model;
	GroupBox * m_midiInputGroupBox;
	LcdSpinBox * m_inputChannelSpinBox;
	LcdSpinBox * m_fixedInputVelocitySpinBox;
	QToolButton * m_rpBtn;

	GroupBox * m_midiOutputGroupBox;
	LcdSpinBox * m_outputChannelSpinBox;
	LcdSpinBox * m_fixedOutputVelocitySpinBox;
	LcdSpinBox * m_outputProgramSpinBox;
	LcdSpinBox * m_fixedOutputNoteSpinBox;
	QToolButton * m_wpBtn;

	LcdSpinBox* m_baseVelocitySpinBox;

} ;


} // namespace gui

} // namespace lmms

#endif // LMMS_GUI_INSTRUMENT_MIDI_IO_VIEW_H
