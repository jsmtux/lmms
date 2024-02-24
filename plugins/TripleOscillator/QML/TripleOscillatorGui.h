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


#ifndef TRIPLE_OSCILLATOR_GUI_H
#define TRIPLE_OSCILLATOR_GUI_H

#include "TripleOscillator.h"
#include "GuiApplication.h"

namespace lmms
{

namespace gui
{

class OscillatorObjectModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(FloatLmmsModel* volume READ volumeModel CONSTANT)
    Q_PROPERTY(FloatLmmsModel* pan READ panModel CONSTANT)
    Q_PROPERTY(FloatLmmsModel* coarse READ coarseModel CONSTANT)
    Q_PROPERTY(FloatLmmsModel* fineLeft READ fineLeftModel CONSTANT)
    Q_PROPERTY(FloatLmmsModel* fineRight READ fineRightModel CONSTANT)
    Q_PROPERTY(FloatLmmsModel* phaseOffset READ phaseOffsetModel CONSTANT)
    Q_PROPERTY(FloatLmmsModel* stereoPhaseDetuning READ stereoPhaseDetuningModel CONSTANT)
    Q_PROPERTY(IntLmmsModel* waveShape READ waveShapeModel CONSTANT)
    Q_PROPERTY(IntLmmsModel* modulationAlgo READ modulationAlgoModel CONSTANT)
    Q_PROPERTY(BoolLmmsModel* useWaveTable READ useWaveTableModel CONSTANT)
public:
    OscillatorObjectModel(QObject* parent, OscillatorObject* _osc) :
        QObject(parent),
        m_osc(_osc)
    {}
    FloatLmmsModel* volumeModel() {
        return &m_volumeModel;
    }
    FloatLmmsModel* panModel() {
        return &m_panModel;
    }
    FloatLmmsModel* coarseModel() {
        return &m_coarseModel;
    }
    FloatLmmsModel* fineLeftModel() {
        return &m_fineLeftModel;
    }
    FloatLmmsModel* fineRightModel() {
        return &m_fineRightModel;
    }
    FloatLmmsModel* phaseOffsetModel() {
        return &m_phaseOffsetModel;
    }
    FloatLmmsModel* stereoPhaseDetuningModel() {
        return &m_stereoPhaseDetuningModel;
    }
    IntLmmsModel* waveShapeModel() {
        return &m_waveShapeModel;
    }
    IntLmmsModel* modulationAlgoModel() {
        return &m_modulationAlgoModel;
    }
    BoolLmmsModel* useWaveTableModel() {
        return &m_useWaveTableModel;
    }
private:
    OscillatorObject* m_osc;
	FloatLmmsModel m_volumeModel{m_osc->m_volumeModel};
	FloatLmmsModel m_panModel{m_osc->m_panModel};
	FloatLmmsModel m_coarseModel{m_osc->m_coarseModel};
	FloatLmmsModel m_fineLeftModel{m_osc->m_fineLeftModel};
	FloatLmmsModel m_fineRightModel{m_osc->m_fineRightModel};
	FloatLmmsModel m_phaseOffsetModel{m_osc->m_phaseOffsetModel};
	FloatLmmsModel m_stereoPhaseDetuningModel{m_osc->m_stereoPhaseDetuningModel};
	IntLmmsModel m_waveShapeModel{m_osc->m_waveShapeModel};
	IntLmmsModel m_modulationAlgoModel{m_osc->m_modulationAlgoModel};
	BoolLmmsModel m_useWaveTableModel{m_osc->m_useWaveTableModel};
};

class TripleOscillatorModel : public InstrumentModel
{
    Q_OBJECT
    Q_PROPERTY(QList<OscillatorObjectModel*> oscillators READ oscillators CONSTANT)
public:
    TripleOscillatorModel(QObject* parent, TripleOscillator* _instrument) :
        InstrumentModel(parent, _instrument),
        m_tripleOscillator(_instrument)
    {
        for(auto i = 0; i < NUM_OF_OSCILLATORS; i++) {
            m_oscillators.push_back(new OscillatorObjectModel(this, m_tripleOscillator->m_osc[i]));
        }
    }

    QList<OscillatorObjectModel*>& oscillators() {
        return m_oscillators;
    }
private:
    TripleOscillator* m_tripleOscillator;
    QList<OscillatorObjectModel*> m_oscillators;
};

class TripleOscillatorQmlInstrument : public TripleOscillator, public ModelFactory {
    Q_OBJECT
public:
    TripleOscillatorQmlInstrument(IInstrumentTrack * _instrument_track)
		: TripleOscillator(_instrument_track)
	{}

	IGUISpecificPlugin* guiSpecificPlugin() override {
        return this;
    }


	InstrumentModel* getModel(QObject* parent) override {
        return new TripleOscillatorModel(parent, this);
    }
};

} // namespace gui

} // namespace lmms

#endif
