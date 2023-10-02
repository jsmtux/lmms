/*
 * flangercontrolsdialog.cpp - defination of FlangerControlsDialog class.
 *
 * Copyright (c) 2014 David French <dave/dot/french3/at/googlemail/dot/com>
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

#include "FlangerControlsDialog.h"

#include "embed.h"
#include "FlangerControls.h"

#include "widgets/LedCheckBox.h"
#include "widgets/TempoSyncKnob.h"

namespace lmms::gui
{


FlangerControlsDialog::FlangerControlsDialog( FlangerControls *controls ) :
	EffectControlDialog( controls )
{
	setAutoFillBackground( true );
	QPalette pal;
	pal.setBrush( backgroundRole(), PLUGIN_NAME::getIconPixmap( "artwork" ) );
	setPalette( pal );
	setFixedSize( 233, 75 );

	auto delayKnob = new Knob(knobBright_26, &controls->m_delayTimeModel, this);
	delayKnob->move( 10,10 );
	delayKnob->setVolumeKnob( false );
	delayKnob->setLabel( tr( "DELAY" ) );
	delayKnob->setHintText( tr( "Delay time:" ) + " ", "s" );

	auto lfoFreqKnob = new TempoSyncKnob(knobBright_26, &controls->m_lfoFrequencyModel, this);
	lfoFreqKnob->move( 48,10 );
	lfoFreqKnob->setVolumeKnob( false );
	lfoFreqKnob->setLabel( tr( "RATE" ) );
	lfoFreqKnob->setHintText( tr( "Period:" ) , " Sec" );

	auto lfoAmtKnob = new Knob(knobBright_26, &controls->m_lfoAmountModel, this);
	lfoAmtKnob->move( 85,10 );
	lfoAmtKnob->setVolumeKnob( false );
	lfoAmtKnob->setLabel( tr( "AMNT" ) );
	lfoAmtKnob->setHintText( tr( "Amount:" ) , "" );

	auto lfoPhaseKnob = new Knob(knobBright_26, &controls->m_lfoPhaseModel, this);
	lfoPhaseKnob->move( 123,10 );
	lfoPhaseKnob->setVolumeKnob( false );
	lfoPhaseKnob->setLabel( tr( "PHASE" ) );
	lfoPhaseKnob->setHintText( tr( "Phase:" ) , " degrees" );

	auto feedbackKnob = new Knob(knobBright_26, &controls->m_feedbackModel, this);
	feedbackKnob->move( 160,10 );
	feedbackKnob->setVolumeKnob( true) ;
	feedbackKnob->setLabel( tr( "FDBK" ) );
	feedbackKnob->setHintText( tr( "Feedback amount:" ) , "" );

	auto whiteNoiseKnob = new Knob(knobBright_26, &controls->m_whiteNoiseAmountModel, this);
	whiteNoiseKnob->move( 196,10 );
	whiteNoiseKnob->setVolumeKnob( true) ;
	whiteNoiseKnob->setLabel( tr( "NOISE" ) );
	whiteNoiseKnob->setHintText( tr( "White noise amount:" ) , "" );

	auto invertCb = new LedCheckBox(tr("Invert"), new BoolModel(false, this), this);
	invertCb->move( 10,53 );



}


} // namespace lmms::gui
