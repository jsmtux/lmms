/*
 * LfoController.h - A LFO-based controller and dialog
 *
 * Copyright (c) 2008 Paul Giblock <drfaygo/at/gmail.com>
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

#ifndef LMMS_LFO_CONTROLLER_H
#define LMMS_LFO_CONTROLLER_H

#include <QWidget>

#include "Model.h"
#include "AutomatableModel.h"
#include "Controller.h"
#include "TempoSyncKnobModel.h"
#include "Oscillator.h"

namespace lmms
{

namespace gui
{

class automatableButtonGroup;
class LedCheckBox;
class PixmapButton;
class Knob;
class TempoSyncKnob;

class LfoControllerDialog;

}


class LfoController : public Controller
{
	Q_OBJECT
public:
	LfoController( Model * _parent );

	~LfoController() override;

	void saveSettings( QDomDocument & _doc, QDomElement & _this ) override;
	void loadSettings( const QDomElement & _this ) override;
	QString nodeName() const override;


protected:
	// The internal per-controller value updating function
	void updateValueBuffer() override;

	FloatModel m_baseModel;
	TempoSyncKnobModel m_speedModel;
	FloatModel m_amountModel;
	FloatModel m_phaseModel;
	IntModel m_waveModel;
	IntModel m_multiplierModel;

	float m_duration;
	float m_phaseOffset;
	float m_currentPhase;

	sample_t (*m_sampleFunction)( const float );

private:
	SampleBuffer * m_userDefSampleBuffer;

protected slots:
	void updatePhase();
	void updateSampleFunction();
	void updateDuration();

	friend class gui::LfoControllerDialog;

} ;


} // namespace lmms

#endif // LMMS_LFO_CONTROLLER_H
