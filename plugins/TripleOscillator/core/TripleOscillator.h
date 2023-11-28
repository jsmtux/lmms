/*
 * TripleOscillator.h - declaration of class TripleOscillator a powerful
 *                      instrument-plugin with 3 oscillators
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

#ifndef _TRIPLE_OSCILLATOR_H
#define _TRIPLE_OSCILLATOR_H

#include "IModels.h"
#include "Instrument.h"

namespace lmms
{


class NotePlayHandle;
class ISampleBuffer;
class IOscillator;



const int NUM_OF_OSCILLATORS = 3;


class OscillatorObject : public Model
{
	Q_OBJECT
public:
	OscillatorObject( QObject * _parent, int _idx );
	~OscillatorObject() override;

	IFloatAutomatableModel* m_volumeModel;
	IFloatAutomatableModel* m_panModel;
	IFloatAutomatableModel* m_coarseModel;
	IFloatAutomatableModel* m_fineLeftModel;
	IFloatAutomatableModel* m_fineRightModel;
	IFloatAutomatableModel* m_phaseOffsetModel;
	IFloatAutomatableModel* m_stereoPhaseDetuningModel;
	IIntAutomatableModel* m_waveShapeModel;
	IIntAutomatableModel* m_modulationAlgoModel;
	IBoolAutomatableModel* m_useWaveTableModel;
	std::unique_ptr<ISampleBuffer> m_sampleBuffer;

	float m_volumeLeft;
	float m_volumeRight;

	// normalized detuning -> x/sampleRate
	float m_detuningLeft;
	float m_detuningRight;
	// normalized offset -> x/360
	float m_phaseOffsetLeft;
	float m_phaseOffsetRight;
	bool m_useWaveTable;

	friend class TripleOscillator;


private slots:
	void oscUserDefWaveDblClick();

	void updateVolume();
	void updateDetuningLeft();
	void updateDetuningRight();
	void updatePhaseOffsetLeft();
	void updatePhaseOffsetRight();
	void updateUseWaveTable();

} ;




class TripleOscillator : public Instrument
{
	Q_OBJECT
public:
	TripleOscillator( IInstrumentTrack * _track );
	~TripleOscillator() override = default;

	void playNote( INotePlayHandle * _n,
						sampleFrame * _working_buffer ) override;
	void deleteNotePluginData( INotePlayHandle * _n ) override;


	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	QString nodeName() const override;

	f_cnt_t desiredReleaseFrames() const override
	{
		return( 128 );
	}


	OscillatorObject * m_osc[NUM_OF_OSCILLATORS];

	struct oscPtr
	{
		IOscillator * oscLeft;
		IOscillator * oscRight;
	} ;

	static const char* PluginName() { return "TripleOscillator"; }


protected slots:
	void updateAllDetuning();
} ;

} // namespace lmms

#endif
