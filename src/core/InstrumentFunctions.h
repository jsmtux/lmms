/*
 * InstrumentFunctions.h - models for instrument-functions-tab
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

#ifndef LMMS_INSTRUMENT_FUNCTIONS_H
#define LMMS_INSTRUMENT_FUNCTIONS_H

#include "JournallingObject.h"
#include "lmms_basics.h"
#include "AutomatableModel.h"
#include "TempoSyncKnobModel.h"
#include "ComboBoxModel.h"
#include "IInstrumentFunctions.h"

namespace lmms
{

class InstrumentTrack;
class NotePlayHandle;


class InstrumentFunctionNoteStacking : public Model, public JournallingObject, public IInstrumentFunctionNoteStacking
{
	Q_OBJECT
public:
	InstrumentFunctionNoteStacking( QObject * _parent );
	~InstrumentFunctionNoteStacking() override = default;

	void processNote( NotePlayHandle* n );


	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	inline QString nodeName() const override
	{
		return "chordcreator";
	}

	IBoolAutomatableModel* chordsEnabledModel() override
	{
		return &m_chordsEnabledModel;
	}
	IComboBoxModelWrapper* chordsModel() override
	{
		return &m_chordsModel;
	}
	IFloatAutomatableModel* chordRangeModel() override
	{
		return &m_chordRangeModel;
	}
	Model* model() override {
		return this;
	}

private:
	BoolModel m_chordsEnabledModel;
	ComboBoxModelWrapper m_chordsModel;
	FloatModel m_chordRangeModel;
} ;




class InstrumentFunctionArpeggio : public Model, public IInstrumentFunctionArpeggio, public JournallingObject
{
	Q_OBJECT
public:
	enum ArpDirections
	{
		ArpDirUp,
		ArpDirDown,
		ArpDirUpAndDown,
		ArpDirDownAndUp,
		ArpDirRandom,
		NumArpDirections
	} ;

	InstrumentFunctionArpeggio( QObject * _parent );
	~InstrumentFunctionArpeggio() override = default;

	void processNote( NotePlayHandle* n );


	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	inline QString nodeName() const override
	{
		return "arpeggiator";
	}


	IBoolAutomatableModel* arpEnabledModel() override
	{
		return &m_arpEnabledModel;
	}
	IComboBoxModelWrapper* arpModel() override
	{
		return &m_arpModel;
	}
	IFloatAutomatableModel* arpRangeModel() override
	{
		return &m_arpRangeModel;
	}
	IFloatAutomatableModel* arpRepeatsModel() override
	{
		return &m_arpRepeatsModel;
	}
	IFloatAutomatableModel* arpCycleModel() override
	{
		return &m_arpCycleModel;
	}
	IFloatAutomatableModel* arpSkipModel() override
	{
		return &m_arpSkipModel;
	}
	IFloatAutomatableModel* arpMissModel() override
	{
		return &m_arpMissModel;
	}
	ITempoSyncKnobModelWrapper* arpTimeModel() override
	{
		return &m_arpTimeModel;
	}
	IFloatAutomatableModel* arpGateModel() override
	{
		return &m_arpGateModel;
	}
	IComboBoxModelWrapper* arpDirectionModel() override
	{
		return &m_arpDirectionModel;
	}
	IComboBoxModelWrapper* arpModeModel() override
	{
		return &m_arpModeModel;
	}
	Model* model() override {
		return this;
	}


private:
	enum ArpModes
	{
		FreeMode,
		SortMode,
		SyncMode
	} ;

	BoolModel m_arpEnabledModel;
	ComboBoxModelWrapper m_arpModel;
	FloatModel m_arpRangeModel;
	FloatModel m_arpRepeatsModel;
	FloatModel m_arpCycleModel;
	FloatModel m_arpSkipModel;
	FloatModel m_arpMissModel;
	TempoSyncKnobModelWrapper m_arpTimeModel;
	FloatModel m_arpGateModel;
	ComboBoxModelWrapper m_arpDirectionModel;
	ComboBoxModelWrapper m_arpModeModel;


	friend class InstrumentTrack;

} ;


} // namespace lmms

#endif // LMMS_INSTRUMENT_FUNCTIONS_H
