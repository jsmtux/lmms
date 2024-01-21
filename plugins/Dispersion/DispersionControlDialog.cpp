/*
 * DispersionControlDialog.cpp
 *
 * Copyright (c) 2023 Lost Robot <r94231/at/gmail/dot/com>
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


#include "DispersionControlDialog.h"
#include "DispersionControls.h"

#include "embed.h"
#include "widgets/Knob.h"
#include "widgets/LcdSpinBox.h"
#include "widgets/PixmapButton.h"


namespace lmms::gui
{


DispersionControlDialog::DispersionControlDialog(DispersionControls* controls) :
	EffectControlDialog(controls)
{
	setAutoFillBackground(true);
	QPalette pal;
	pal.setBrush(backgroundRole(), PLUGIN_NAME::getIconPixmap("artwork"));
	setPalette(pal);
	setFixedSize(207, 50);

	LcdSpinBox * m_amountBox = new LcdSpinBox(3, &controls->m_amountModel, this, "Amount");
	m_amountBox->move(5, 10);
	m_amountBox->setLabel(tr("AMOUNT"));
	m_amountBox->setToolTip(tr("Number of all-pass filters"));
	
	Knob * freqKnob = new Knob(knobBright_26, &controls->m_freqModel, this);
	freqKnob->move(59, 8);
	freqKnob->setLabel(tr("FREQ"));
	freqKnob->setHintText(tr("Frequency:") , " Hz");
	
	Knob * resoKnob = new Knob(knobBright_26, &controls->m_resoModel, this);
	resoKnob->move(99, 8);
	resoKnob->setLabel(tr("RESO"));
	resoKnob->setHintText(tr("Resonance:") , " octaves");
	
	Knob * feedbackKnob = new Knob(knobBright_26, &controls->m_feedbackModel, this);
	feedbackKnob->move(139, 8);
	feedbackKnob->setLabel(tr("FEED"));
	feedbackKnob->setHintText(tr("Feedback:") , "");
	
	PixmapButton * dcButton = new PixmapButton(this, &controls->m_dcModel, tr("DC Offset Removal"));
	dcButton->move(176, 16);
	dcButton->setActiveGraphic(PLUGIN_NAME::getIconPixmap("dc_active"));
	dcButton->setInactiveGraphic(PLUGIN_NAME::getIconPixmap("dc_inactive"));
	dcButton->setCheckable(true);
	dcButton->setToolTip(tr("Remove DC Offset"));
}


} // namespace lmms::gui
