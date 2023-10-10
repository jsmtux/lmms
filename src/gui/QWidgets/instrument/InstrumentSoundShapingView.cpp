/*
 * InstrumentSoundShapingView.cpp - view for InstrumentSoundShaping class
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

#include <QLabel>

#include "InstrumentSoundShapingView.h"

#include "EnvelopeAndLfoParameters.h"
#include "EnvelopeAndLfoView.h"
#include "gui_templates.h"

#include "widgets/ComboBox.h"
#include "widgets/GroupBox.h"
#include "widgets/Knob.h"
#include "widgets/TabWidget.h"


namespace lmms::gui
{

const int TARGETS_TABWIDGET_X = 4;
const int TARGETS_TABWIDGET_Y = 5;
const int TARGETS_TABWIDGET_WIDTH = 242;
const int TARGETS_TABWIDGET_HEIGTH = 175;

const int FILTER_GROUPBOX_X = TARGETS_TABWIDGET_X;
const int FILTER_GROUPBOX_Y = TARGETS_TABWIDGET_Y+TARGETS_TABWIDGET_HEIGTH+5;
const int FILTER_GROUPBOX_WIDTH = TARGETS_TABWIDGET_WIDTH;
const int FILTER_GROUPBOX_HEIGHT = 245-FILTER_GROUPBOX_Y;



InstrumentSoundShapingView::InstrumentSoundShapingView( InstrumentSoundShaping* m_ss, QWidget * _parent ) :
	QWidget( _parent ),
	m_ss( m_ss )
{
	m_targetsTabWidget = new TabWidget( tr( "TARGET" ), this );
	m_targetsTabWidget->setGeometry( TARGETS_TABWIDGET_X,
						TARGETS_TABWIDGET_Y,
						TARGETS_TABWIDGET_WIDTH,
						TARGETS_TABWIDGET_HEIGTH );

	for( int i = 0; i < InstrumentSoundShaping::NumTargets; ++i )
	{
		m_envLfoViews[i] = new EnvelopeAndLfoView(m_ss->m_envLfoParameters[i], m_targetsTabWidget);
		m_targetsTabWidget->addTab( m_envLfoViews[i],
						tr( InstrumentSoundShaping::targetNames[i][0] ), 
                                                nullptr );
	}


	m_filterGroupBox = new GroupBox( tr( "FILTER" ), &m_ss->m_filterEnabledModel, this );
	m_filterGroupBox->setGeometry( FILTER_GROUPBOX_X, FILTER_GROUPBOX_Y,
						FILTER_GROUPBOX_WIDTH,
						FILTER_GROUPBOX_HEIGHT );


	m_filterComboBox = new ComboBox( &m_ss->m_filterModel, m_filterGroupBox );
	m_filterComboBox->setGeometry( 14, 22, 120, ComboBox::DEFAULT_HEIGHT );
	m_filterComboBox->setFont( pointSize<8>( m_filterComboBox->font() ) );


	m_filterCutKnob = new Knob( knobBright_26, &m_ss->m_filterCutModel, m_filterGroupBox );
	m_filterCutKnob->setLabel( tr( "FREQ" ) );
	m_filterCutKnob->move( 140, 18 );
	m_filterCutKnob->setHintText( tr( "Cutoff frequency:" ), " " + tr( "Hz" ) );


	m_filterResKnob = new Knob( knobBright_26, &m_ss->m_filterResModel, m_filterGroupBox );
	m_filterResKnob->setLabel( tr( "Q/RESO" ) );
	m_filterResKnob->move( 196, 18 );
	m_filterResKnob->setHintText( tr( "Q/Resonance:" ), "" );


	m_singleStreamInfoLabel = new QLabel( tr( "Envelopes, LFOs and filters are not supported by the current instrument." ), this );
	m_singleStreamInfoLabel->setWordWrap( true );
	m_singleStreamInfoLabel->setFont( pointSize<8>( m_singleStreamInfoLabel->font() ) );

	m_singleStreamInfoLabel->setGeometry( TARGETS_TABWIDGET_X,
						TARGETS_TABWIDGET_Y,
						TARGETS_TABWIDGET_WIDTH,
						TARGETS_TABWIDGET_HEIGTH );

	QObject::connect( m_ss, SIGNAL(dataChanged()), this, SLOT(update()));
	QObject::connect( m_ss, SIGNAL(propertiesChanged()), this, SLOT(update()));

	update();
}




InstrumentSoundShapingView::~InstrumentSoundShapingView()
{
	delete m_targetsTabWidget;
}



void InstrumentSoundShapingView::setFunctionsHidden( bool hidden )
{
	m_targetsTabWidget->setHidden( hidden );
	m_filterGroupBox->setHidden( hidden );
	m_singleStreamInfoLabel->setHidden( !hidden );
}




} // namespace lmms::gui
