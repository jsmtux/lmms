/*
 * Kicker.h - drum synthesizer
 *
 * Copyright (c) 2006-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2014 grejppi <grejppi/at/gmail.com>
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


#ifndef KICKER_H
#define KICKER_H

#include "IModels.h"
#include "Instrument.h"


namespace lmms
{

#define KICKER_PRESET_VERSION 1


class NotePlayHandle;


class KickerInstrument : public Instrument
{
	Q_OBJECT
public:
	KickerInstrument( IInstrumentTrack * _instrument_track );
	~KickerInstrument() override = default;

	void playNote( INotePlayHandle * _n,
						sampleFrame * _working_buffer ) override;
	void deleteNotePluginData( INotePlayHandle * _n ) override;

	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	QString nodeName() const override;

	Flags flags() const override
	{
		return IsNotBendable;
	}

	f_cnt_t desiredReleaseFrames() const override
	{
		return( 512 );
	}

	static const char* PluginName() { return "Kicker"; }

	IFloatAutomatableModel* m_startFreqModel;
	IFloatAutomatableModel* m_endFreqModel;
	ITempoSyncKnobModelWrapper* m_decayModel;
	IFloatAutomatableModel* m_distModel;
	IFloatAutomatableModel* m_distEndModel;
	IFloatAutomatableModel* m_gainModel;
	IFloatAutomatableModel* m_envModel;
	IFloatAutomatableModel* m_noiseModel;
	IFloatAutomatableModel* m_clickModel;
	IFloatAutomatableModel* m_slopeModel;

	IBoolAutomatableModel* m_startNoteModel;
	IBoolAutomatableModel* m_endNoteModel;

	IIntAutomatableModel* m_versionModel;
	IInstrumentTrack * m_instrument_track;
} ;

} // namespace lmms

#endif
