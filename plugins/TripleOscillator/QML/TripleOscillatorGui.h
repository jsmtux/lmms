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
public:
    OscillatorObjectModel(QObject* parent, OscillatorObject* _osc) :
        QObject(parent),
        m_osc(_osc)
    {

    }
private:
    OscillatorObject* m_osc;
	IFloatAutomatableModel* m_volumeModel{m_osc->m_volumeModel};
    EXPOSE_LMMS_PROPERTY(float, volume, m_volumeModel)
	IFloatAutomatableModel* m_panModel{m_osc->m_panModel};
    EXPOSE_LMMS_PROPERTY(float, pan, m_panModel)
	IFloatAutomatableModel* m_coarseModel{m_osc->m_coarseModel};
    EXPOSE_LMMS_PROPERTY(float, coarse, m_coarseModel)
	IFloatAutomatableModel* m_fineLeftModel{m_osc->m_fineLeftModel};
    EXPOSE_LMMS_PROPERTY(float, fineLeft, m_fineLeftModel)
	IFloatAutomatableModel* m_fineRightModel{m_osc->m_fineRightModel};
    EXPOSE_LMMS_PROPERTY(float, fineRight, m_fineRightModel)
	IFloatAutomatableModel* m_phaseOffsetModel{m_osc->m_phaseOffsetModel};
    EXPOSE_LMMS_PROPERTY(float, phaseOffset, m_phaseOffsetModel)
	IFloatAutomatableModel* m_stereoPhaseDetuningModel{m_osc->m_stereoPhaseDetuningModel};
    EXPOSE_LMMS_PROPERTY(float, stereoPhaseDetuning, m_stereoPhaseDetuningModel)
	IIntAutomatableModel* m_waveShapeModel{m_osc->m_waveShapeModel};
    EXPOSE_LMMS_PROPERTY(int, waveShape, m_waveShapeModel)
	IIntAutomatableModel* m_modulationAlgoModel{m_osc->m_modulationAlgoModel};
    EXPOSE_LMMS_PROPERTY(int, modulationAlgo, m_modulationAlgoModel)
	IBoolAutomatableModel* m_useWaveTableModel{m_osc->m_useWaveTableModel};
    EXPOSE_LMMS_PROPERTY(bool, useWaveTable, m_useWaveTableModel)
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
