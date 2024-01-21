/*
 * ControllerConnectionDialog.cpp - dialog allowing the user to create and
 *	modify links between controllers and models
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

#include "ControllerConnectionDialog.h"

#include "IAudioEngine.h"
#include "IControllerConnection.h"
#include "embed.h"
// #include "MidiController.h"
#include "IMidiClient.h"
#include "ISong.h"
#include "IEngine.h"
#include "IMidiController.h"

#include "menus/MidiPortMenu.h"

#include "widgets/ComboBox.h"
#include "widgets/GroupBox.h"
#include "widgets/LcdSpinBox.h"
#include "widgets/LedCheckBox.h"
#include "widgets/TabWidget.h"
#include "widgets/ToolButton.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>


namespace lmms
{

namespace gui
{

ControllerConnectionDialog::ControllerConnectionDialog( QWidget * _parent, 
		const IAutomatableModelBase * _target_model ) :
	QDialog( _parent ),
	m_readablePorts( nullptr ),
	m_midiAutoDetect( MFact::create(false) ),
	m_controller( nullptr ),
	m_targetModel( _target_model ),
	m_midiController( createAutoDetectMidiController( IEngine::Instance()->getSongInterface() ) )
{
	setWindowIcon( embed::getIconPixmap( "setup_audio" ) );
	setWindowTitle( tr( "Connection Settings" ) );
	setModal( true );

	// Midi stuff
	IMidiPort::Map map = m_midiController->midiPortInterface()->readablePorts();
	for( IMidiPort::Map::Iterator it = map.begin(); it != map.end(); ++it )
	{
		it.value() = true;
	}
	m_midiController->subscribeReadablePorts( map );


	connect( m_midiController->controllerInterface(), &IController::valueChanged, this, &ControllerConnectionDialog::midiValueChanged);

	m_midiGroupBox = new GroupBox( tr( "MIDI CONTROLLER" ), MFact::create(false, this), this );
	m_midiGroupBox->setGeometry( 8, 10, 240, 80 );
	connect( m_midiGroupBox->model()->model(), &Model::dataChanged,
			this, &ControllerConnectionDialog::midiToggled);
	
	m_midiChannelSpinBox = new LcdSpinBox( 2,
			m_midiController->midiPortInterface()->inputChannelModel(),
			m_midiGroupBox, tr( "Input channel" ) );
	m_midiChannelSpinBox->addTextForValue( 0, "--" );
	m_midiChannelSpinBox->setLabel( tr( "CHANNEL" ) );
	m_midiChannelSpinBox->move( 8, 24 );

	m_midiControllerSpinBox = new LcdSpinBox( 3,  m_midiController->midiPortInterface()->inputControllerModel(),
			m_midiGroupBox,	tr( "Input controller" ) );
	m_midiControllerSpinBox->addTextForValue( 0, "---" );
	m_midiControllerSpinBox->setLabel( tr( "CONTROLLER" ) );
	m_midiControllerSpinBox->move( 68, 24 );
	

	m_midiAutoDetectCheckBox =
			new LedCheckBox( tr("Auto Detect"),
			m_midiAutoDetect.get(), m_midiGroupBox, tr("Auto Detect") );
	m_midiAutoDetectCheckBox->move( 8, 60 );
	connect( m_midiAutoDetect->model(), &Model::dataChanged,
			this, &ControllerConnectionDialog::autoDetectToggled);

	// when using with non-raw-clients we can provide buttons showing
	// our port-menus when being clicked
	if( !IEngine::Instance()->getAudioEngineInterface()->midiClientInterface()->isRaw() )
	{
		m_readablePorts = new MidiPortMenu( IMidiPort::Input, m_midiController->midiPortInterface());
		connect( m_readablePorts, SIGNAL(triggered(QAction*)),
				this, SLOT(enableAutoDetect(QAction*)));
		auto rp_btn = new ToolButton(m_midiGroupBox);
		rp_btn->setText( tr( "MIDI-devices to receive "
						"MIDI-events from" ) );
		rp_btn->setIcon( embed::getIconPixmap( "piano" ) );
		rp_btn->setGeometry( 160, 24, 32, 32 );
		rp_btn->setMenu( m_readablePorts );
		rp_btn->setPopupMode( QToolButton::InstantPopup );
	}


	// User stuff
	m_userGroupBox = new GroupBox( tr( "USER CONTROLLER" ), MFact::create(false, this), this );
	m_userGroupBox->setGeometry( 8, 100, 240, 60 );
	connect( m_userGroupBox->model()->model(), SIGNAL(dataChanged()),
			this, SLOT(userToggled()));

	m_userController = new ComboBox( MFact::createComboBox(this), m_userGroupBox, "Controller" );
	m_userController->setGeometry( 10, 24, 200, ComboBox::DEFAULT_HEIGHT );
	for (auto * c : IEngine::Instance()->getSongInterface()->controllers())
	{
		m_userController->comboboxModel()->addItem( c->name() );
	}
	connect( m_userController->model()->model(), &Model::dataUnchanged,
			this, &ControllerConnectionDialog::userSelected);
	connect( m_userController->model()->model(), &Model::dataChanged,
			this, &ControllerConnectionDialog::userSelected);


	// Mapping functions
	m_mappingBox = new TabWidget( tr( "MAPPING FUNCTION" ), this );
	m_mappingBox->setGeometry( 8, 170, 240, 64 );
	m_mappingFunction = new QLineEdit( m_mappingBox );
	m_mappingFunction->setGeometry( 10, 20, 170, 16 );
	m_mappingFunction->setText( "input" );
	m_mappingFunction->setReadOnly( true );


	// Buttons
	auto buttons = new QWidget(this);
	buttons->setGeometry( 8, 240, 240, 32 );

	auto btn_layout = new QHBoxLayout(buttons);
	btn_layout->setSpacing( 0 );
	btn_layout->setContentsMargins(0, 0, 0, 0);

	auto select_btn = new QPushButton(embed::getIconPixmap("add"), tr("OK"), buttons);
	connect( select_btn, SIGNAL(clicked()), 
				this, SLOT(selectController()));

	auto cancel_btn = new QPushButton(embed::getIconPixmap("cancel"), tr("Cancel"), buttons);
	connect( cancel_btn, SIGNAL(clicked()),
				this, SLOT(reject()));

	btn_layout->addStretch();
	btn_layout->addSpacing( 10 );
	btn_layout->addWidget( select_btn );
	btn_layout->addSpacing( 10 );
	btn_layout->addWidget( cancel_btn );
	btn_layout->addSpacing( 10 );

	setFixedSize( 256, 280 );

	// Crazy MIDI View stuff
	
	// TODO, handle by making this a model for the Dialog "view"
	IControllerConnection * cc = nullptr;
	if( m_targetModel )
	{
		cc = m_targetModel->controllerConnection();

		if( cc && cc->getController()->type() != IController::DummyController && IEngine::Instance()->getSongInterface() )
		{
			if ( cc->getController()->type() == IController::MidiController )
			{
				m_midiGroupBox->model()->setValue( true );
				// ensure controller is created
				midiToggled();

				auto cont = getMidiControllerFromBaseController(cc->getController());
				m_midiChannelSpinBox->model()->setValue( cont->midiPortInterface()->inputChannelModel()->value() );
				m_midiControllerSpinBox->model()->setValue( cont->midiPortInterface()->inputControllerModel()->value() );

				m_midiController->subscribeReadablePorts( getMidiControllerFromBaseController( cc->getController() )->midiPortInterface()->readablePorts() );
			}
			else
			{
				int idx = IEngine::Instance()->getSongInterface()->controllers().indexOf( cc->getController() );

				if( idx >= 0 )
				{
					m_userGroupBox->model()->setValue( true );
					m_userController->model()->setValue( idx );
				}
			}
		}
	}

	if( !cc )
	{
		m_midiGroupBox->model()->setValue( true );
	}

	show();
}




ControllerConnectionDialog::~ControllerConnectionDialog()
{
	delete m_readablePorts;
}




void ControllerConnectionDialog::selectController()
{
	// Midi
	if( m_midiGroupBox->model()->value() > 0 )
	{
		if( m_midiControllerSpinBox->model()->value() > 0 )
		{
			IMidiController * mc;
			mc = m_midiController->copyToMidiController( &IEngine::Instance()->getSongInterface()->trackContainer() );
	
			/*
			if( m_targetModel->getTrack() && 
					!m_targetModel->getTrack()->displayName().isEmpty() )
			{
				mc->midiPortInterface()->setName( QString( "%1 (%2)" ).
						arg( m_targetModel->getTrack()->displayName() ).
						arg( m_targetModel->displayName() ) );
			}
			else
			{
				mc->midiPortInterface()->setName( m_targetModel->displayName() );
			}
			*/
			mc->midiPortInterface()->setName( m_targetModel->model()->fullDisplayName() );
			m_controller = mc->baseController();
		}
	}
	// User
	else 
	{
		if( m_userGroupBox->model()->value() > 0 && 
				IEngine::Instance()->getSongInterface()->controllers().size() )
		{
			m_controller = IEngine::Instance()->getSongInterface()->controllers().at( 
					m_userController->model()->value() );
		}

		if( m_controller && m_controller->hasModel( m_targetModel->model() ) )
		{
			QMessageBox::warning(this, tr("LMMS"), tr("Cycle Detected."));
			return;
		}
	
	}

	accept();
}




void ControllerConnectionDialog::midiToggled()
{
	int enabled = m_midiGroupBox->model()->value();
	if( enabled != 0 )
	{
		if( m_userGroupBox->model()->value() != 0 )
		{
			m_userGroupBox->model()->setValue( 0 );
		}

		if( !m_midiController )
		{
			qWarning("Before refactor, midi controller was queried here\n");
		}
	}
	m_midiAutoDetect->setValue( enabled );

	if (m_midiChannelSpinBox) {
		m_midiChannelSpinBox->setEnabled( enabled );
		m_midiControllerSpinBox->setEnabled( enabled );
		m_midiAutoDetectCheckBox->setEnabled( enabled );
	}
}




void ControllerConnectionDialog::userToggled()
{
	int enabled = m_userGroupBox->model()->value();
	if( enabled != 0 && m_midiGroupBox->model()->value() != 0 )
	{
		m_midiGroupBox->model()->setValue( 0 );
	}
}




void ControllerConnectionDialog::userSelected()
{
	m_userGroupBox->model()->setValue( 1 );
	userToggled();
}




void ControllerConnectionDialog::autoDetectToggled()
{
	if( m_midiAutoDetect->value() )
	{
		m_midiController->getAutodetectIfExists()->reset();
	}
}




void ControllerConnectionDialog::midiValueChanged()
{
	if( m_midiAutoDetect->value() )
	{
		m_midiController->getAutodetectIfExists()->useDetected();
		if( m_readablePorts )
		{
			m_readablePorts->updateMenu();
		}
	}
}



void ControllerConnectionDialog::enableAutoDetect( QAction * _a )
{
	if( _a->isChecked() )
	{
		m_midiAutoDetectCheckBox->model()->setValue( true );
	}
}



} // namespace gui

} // namespace lmms
