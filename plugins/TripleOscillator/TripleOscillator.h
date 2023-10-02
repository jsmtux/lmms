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

#include "AutomatableModel.h"

#include "instrument/InstrumentView.h"

#include "plugins/QWidgetInstrumentPlugin.h"

namespace lmms
{


class NotePlayHandle;
class SampleBuffer;
class Oscillator;


namespace gui
{
class automatableButtonGroup;
class Knob;
class PixmapButton;
class TripleOscillatorView;
} // namespace gui


const int NUM_OF_OSCILLATORS = 3;


class OscillatorObject : public Model
{
	MM_OPERATORS
	Q_OBJECT
public:
	OscillatorObject( Model * _parent, int _idx );
	~OscillatorObject() override;


private:
	FloatModel m_volumeModel;
	FloatModel m_panModel;
	FloatModel m_coarseModel;
	FloatModel m_fineLeftModel;
	FloatModel m_fineRightModel;
	FloatModel m_phaseOffsetModel;
	FloatModel m_stereoPhaseDetuningModel;
	IntModel m_waveShapeModel;
	IntModel m_modulationAlgoModel;
	BoolModel m_useWaveTableModel;
	SampleBuffer* m_sampleBuffer;

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
	friend class gui::TripleOscillatorView;


private slots:
	void oscUserDefWaveDblClick();

	void updateVolume();
	void updateDetuningLeft();
	void updateDetuningRight();
	void updatePhaseOffsetLeft();
	void updatePhaseOffsetRight();
	void updateUseWaveTable();

} ;




class TripleOscillator : public gui::QWidgetInstrumentPlugin
{
	Q_OBJECT
public:
	TripleOscillator( InstrumentTrack * _track );
	~TripleOscillator() override = default;

	void playNote( NotePlayHandle * _n,
						sampleFrame * _working_buffer ) override;
	void deleteNotePluginData( NotePlayHandle * _n ) override;


	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	QString nodeName() const override;

	f_cnt_t desiredReleaseFrames() const override
	{
		return( 128 );
	}

	gui::InstrumentView* instantiateView( QWidget * _parent ) override;


protected slots:
	void updateAllDetuning();


private:
	OscillatorObject * m_osc[NUM_OF_OSCILLATORS];

	struct oscPtr
	{
		MM_OPERATORS
		Oscillator * oscLeft;
		Oscillator * oscRight;
	} ;


	friend class gui::TripleOscillatorView;

} ;


namespace gui
{


class TripleOscillatorView : public InstrumentViewImpl<TripleOscillator>
{
	Q_OBJECT
public:
	TripleOscillatorView( TripleOscillator * _instrument, QWidget * _parent );
	~TripleOscillatorView() override = default;


private:
	automatableButtonGroup * m_mod1BtnGrp;
	automatableButtonGroup * m_mod2BtnGrp;
} ;


} // namespace gui

} // namespace lmms

#endif
