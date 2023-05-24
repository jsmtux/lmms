/*
 * InstrumentMidiIOView.cpp - MIDI-IO-View
 *
 * Copyright (c) 2005-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "InstrumentMidiIOView.h"

#include "IAudioEngine.h"
#include "IEngine.h"
#include "embed.h"
#include "gui_templates.h"
// #include "MidiClient.h"
#include "IMidiPort.h"

#include "menus/MidiPortMenu.h"

#include "widgets/GroupBox.h"
#include "widgets/LcdSpinBox.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace lmms::gui
{


InstrumentMidiIOView::InstrumentMidiIOView( IMidiPort* mp, QWidget* parent ) :
	QWidget( parent ),
	m_model(mp),
	m_rpBtn( nullptr ),
	m_wpBtn( nullptr )
{
	auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(5, 5, 5, 5);
	m_midiInputGroupBox = new GroupBox( tr( "ENABLE MIDI INPUT" ), mp->readableModel() );
	layout->addWidget( m_midiInputGroupBox );

	auto midiInputLayout = new QHBoxLayout(m_midiInputGroupBox);
	midiInputLayout->setContentsMargins( 8, 18, 8, 8 );
	midiInputLayout->setSpacing( 4 );

	m_inputChannelSpinBox = new LcdSpinBox( 2, mp->inputChannelModel(), m_midiInputGroupBox );
	m_inputChannelSpinBox->addTextForValue( 0, "--" );
	/*: This string must be be short, its width must be less than
	 *  width of LCD spin-box of two digits */
	m_inputChannelSpinBox->setLabel( tr( "CHAN" ) );
	m_inputChannelSpinBox->setEnabled( false );
	midiInputLayout->addWidget( m_inputChannelSpinBox );

	m_fixedInputVelocitySpinBox = new LcdSpinBox( 3, mp->fixedInputVelocityModel(), m_midiInputGroupBox );
	m_fixedInputVelocitySpinBox->addTextForValue( -1, "---" );
	/*: This string must be be short, its width must be less than
	 *  width of LCD spin-box of three digits */
	m_fixedInputVelocitySpinBox->setLabel( tr( "VELOC" ) );
	m_fixedInputVelocitySpinBox->setEnabled( false );
	midiInputLayout->addWidget( m_fixedInputVelocitySpinBox );
	midiInputLayout->addStretch();

	connect( m_midiInputGroupBox->ledButton(), SIGNAL(toggled(bool)),
			m_inputChannelSpinBox, SLOT(setEnabled(bool)));
	connect( m_midiInputGroupBox->ledButton(), SIGNAL(toggled(bool)),
		m_fixedInputVelocitySpinBox, SLOT(setEnabled(bool)));



	m_midiOutputGroupBox = new GroupBox( tr( "ENABLE MIDI OUTPUT" ), mp->writableModel() );
	layout->addWidget( m_midiOutputGroupBox );

	auto midiOutputLayout = new QHBoxLayout(m_midiOutputGroupBox);
	midiOutputLayout->setContentsMargins( 8, 18, 8, 8 );
	midiOutputLayout->setSpacing( 4 );

	m_outputChannelSpinBox = new LcdSpinBox( 2, mp->outputChannelModel(), m_midiOutputGroupBox );
	m_outputChannelSpinBox->addTextForValue( 0, "--" );
	/*: This string must be be short, its width must be less than
	 *  width of LCD spin-box of two digits */
	m_outputChannelSpinBox->setLabel( tr( "CHAN" ) );
	midiOutputLayout->addWidget( m_outputChannelSpinBox );

	m_fixedOutputVelocitySpinBox = new LcdSpinBox( 3, mp->fixedOutputVelocityModel(), m_midiOutputGroupBox );
	m_fixedOutputVelocitySpinBox->addTextForValue( -1, "---" );
	/*: This string must be be short, its width must be less than
	 *  width of LCD spin-box of three digits */
	m_fixedOutputVelocitySpinBox->setLabel( tr( "VELOC" ) );
	m_fixedOutputVelocitySpinBox->setEnabled( false );
	midiOutputLayout->addWidget( m_fixedOutputVelocitySpinBox );

	m_outputProgramSpinBox = new LcdSpinBox( 3, mp->outputProgramModel(), m_midiOutputGroupBox );
	/*: This string must be be short, its width must be less than the
	 *  width of LCD spin-box of three digits */
	m_outputProgramSpinBox->setLabel( tr( "PROG" ) );
	m_outputProgramSpinBox->setEnabled( false );
	midiOutputLayout->addWidget( m_outputProgramSpinBox );

	m_fixedOutputNoteSpinBox = new LcdSpinBox( 3, mp->fixedOutputNoteModel(), m_midiOutputGroupBox );
	m_fixedOutputNoteSpinBox->addTextForValue( -1, "---" );
	/*: This string must be be short, its width must be less than
	 *  width of LCD spin-box of three digits */
	m_fixedOutputNoteSpinBox->setLabel( tr( "NOTE" ) );
	m_fixedOutputNoteSpinBox->setEnabled( false );
	midiOutputLayout->addWidget( m_fixedOutputNoteSpinBox );
	midiOutputLayout->addStretch();

	connect( m_midiOutputGroupBox->ledButton(), SIGNAL(toggled(bool)),
		m_fixedOutputVelocitySpinBox, SLOT(setEnabled(bool)));
	connect( m_midiOutputGroupBox->ledButton(), SIGNAL(toggled(bool)),
			m_outputProgramSpinBox, SLOT(setEnabled(bool)));
	connect( m_midiOutputGroupBox->ledButton(), SIGNAL(toggled(bool)),
		m_fixedOutputNoteSpinBox, SLOT(setEnabled(bool)));

	if( !IEngine::Instance()->getAudioEngineInterface()->midiClientInterface()->isRaw() )
	{
		m_rpBtn = new QToolButton;
		m_rpBtn->setMinimumSize( 32, 32 );
		m_rpBtn->setText( tr( "MIDI devices to receive MIDI events from" ) );
		m_rpBtn->setIcon( embed::getIconPixmap( "piano" ) );
		m_rpBtn->setPopupMode( QToolButton::InstantPopup );
		midiInputLayout->insertSpacing( 0, 4 );
		midiInputLayout->insertWidget( 0, m_rpBtn );

		m_wpBtn = new QToolButton;
		m_wpBtn->setMinimumSize( 32, 32 );
		m_wpBtn->setText( tr( "MIDI devices to send MIDI events to" ) );
		m_wpBtn->setIcon( embed::getIconPixmap( "piano" ) );
		m_wpBtn->setPopupMode( QToolButton::InstantPopup );
		midiOutputLayout->insertSpacing( 0, 4 );
		midiOutputLayout->insertWidget( 0, m_wpBtn );
	}

	auto* baseVelocityGroupBoxModel = MFact::create(false, this);
	auto baseVelocityGroupBox = new GroupBox(tr("CUSTOM BASE VELOCITY"), baseVelocityGroupBoxModel);
	layout->addWidget( baseVelocityGroupBox );

	auto baseVelocityLayout = new QVBoxLayout(baseVelocityGroupBox);
	baseVelocityLayout->setContentsMargins( 8, 18, 8, 8 );
	baseVelocityLayout->setSpacing( 6 );

	auto baseVelocityHelp
		= new QLabel(tr("Specify the velocity normalization base for MIDI-based instruments at 100% note velocity."));
	baseVelocityHelp->setWordWrap( true );
    baseVelocityHelp->setFont( pointSize<8>( baseVelocityHelp->font() ) );

	baseVelocityLayout->addWidget( baseVelocityHelp );

	m_baseVelocitySpinBox = new LcdSpinBox( 3, mp->baseVelocityModel(), baseVelocityGroupBox );
	m_baseVelocitySpinBox->setLabel( tr( "BASE VELOCITY" ) );
	m_baseVelocitySpinBox->setEnabled( false );
	baseVelocityLayout->addWidget( m_baseVelocitySpinBox );

	connect( baseVelocityGroupBox->ledButton(), SIGNAL(toggled(bool)),
			m_baseVelocitySpinBox, SLOT(setEnabled(bool)));

	layout->addStretch();

	QObject::connect( m_model->model(), SIGNAL(dataChanged()), this, SLOT(update()));
	QObject::connect( m_model->model(), SIGNAL(propertiesChanged()), this, SLOT(update()));

	if( m_rpBtn )
	{
		m_rpBtn->setMenu( mp->readablePortsMenu() );
	}
	if( m_wpBtn )
	{
		m_wpBtn->setMenu( mp->writablePortsMenu() );
	}

	update();
}

} // namespace lmms::gui
