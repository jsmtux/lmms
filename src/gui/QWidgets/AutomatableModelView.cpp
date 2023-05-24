/*
 * AutomatableModelView.cpp - implementation of AutomatableModelView
 *
 * Copyright (c) 2011-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "AutomatableModelView.h"

#include "embed.h"
#include "GuiApplication.h"
#include "MainWindow.h"
#include "StringPairDrag.h"
#include "Clipboard.h"
#include "IControllerConnection.h"

#include "editors/AutomationEditor.h"
#include "modals/ControllerConnectionDialog.h"

#include <QMenu>
#include <QMouseEvent>


namespace lmms::gui
{

static float floatFromClipboard(bool* ok=nullptr);

AutomatableModelViewBase::AutomatableModelViewBase( IAutomatableModelBase* _model, QWidget* _widget ) :
	m_widget( _widget ),
	m_conversionFactor( 1.0 ),
	m_model( _model )
{
	_widget->setAcceptDrops( true );
	_widget->setCursor( QCursor( embed::getIconPixmap( "hand" ), 3, 3 ) );

	// QObject::connect( m_model, &Model::dataChanged, m_widget, &QWidget::update);
	// QObject::connect( m_model, &Model::propertiesChanged, m_widget, &QWidget::update);

	_widget->update();
}

void AutomatableModelViewBase::addDefaultActions( QMenu* menu )
{
	auto amvSlots = new AutomatableModelViewSlots(this, menu);

	menu->addAction( embed::getIconPixmap( "reload" ),
						QObject::tr( "&Reset (%1%2)" ).
							arg( m_model->initValueAsFloat() * m_conversionFactor ).
							arg( m_unit ),
						m_model->model(), SLOT(reset()));

	menu->addSeparator();
	menu->addAction( embed::getIconPixmap( "edit_copy" ),
						QObject::tr( "&Copy value (%1%2)" ).
							arg( m_model->valueAsFloat() * m_conversionFactor ).
							arg( m_unit ),
						amvSlots, SLOT(copyToClipboard()));

	bool canPaste = true;
	const float valueToPaste = floatFromClipboard(&canPaste);
	const QString pasteDesc = canPaste ?
					QObject::tr( "&Paste value (%1%2)").
						arg( valueToPaste ).
						arg( m_unit )
					: QObject::tr( "&Paste value");
	QAction* pasteAction = menu->addAction( embed::getIconPixmap( "edit_paste" ),
						pasteDesc, amvSlots, SLOT(pasteFromClipboard()));
	pasteAction->setEnabled(canPaste);

	menu->addSeparator();

	menu->addAction( embed::getIconPixmap( "automation" ),
						QObject::tr( "Edit song-global automation" ),
							amvSlots,
							SLOT(editSongGlobalAutomation()));

	menu->addAction( QPixmap(),
						QObject::tr( "Remove song-global automation" ),
						amvSlots,
						SLOT(removeSongGlobalAutomation()));

	menu->addSeparator();

	if( m_model->hasLinkedModels() )
	{
		menu->addAction( embed::getIconPixmap( "edit-delete" ),
							QObject::tr( "Remove all linked controls" ),
							amvSlots, SLOT(unlinkAllModels()));
		menu->addSeparator();
	}

	QString controllerTxt;
	if( m_model->controllerConnection() )
	{
		IController* cont = m_model->controllerConnection()->getController();
		if( cont )
		{
			controllerTxt = QObject::tr( "Connected to %1" ).arg( cont->name() );
		}
		else
		{
			controllerTxt = QObject::tr( "Connected to controller" );
		}

		QMenu* contMenu = menu->addMenu( embed::getIconPixmap( "controller" ), controllerTxt );

		contMenu->addAction( embed::getIconPixmap( "controller" ),
								QObject::tr("Edit connection..."),
								amvSlots, SLOT(execConnectionDialog()));
		contMenu->addAction( embed::getIconPixmap( "cancel" ),
								QObject::tr("Remove connection"),
								amvSlots, SLOT(removeConnection()));
	}
	else
	{
		menu->addAction( embed::getIconPixmap( "controller" ),
							QObject::tr("Connect to controller..."),
							amvSlots, SLOT(execConnectionDialog()));
	}
}


void AutomatableModelViewBase::mousePressEvent( QMouseEvent* event )
{
	if( event->button() == Qt::LeftButton && event->modifiers() & Qt::ControlModifier )
	{
		new gui::StringPairDrag( "automatable_model", QString::number( m_model->id() ), QPixmap(), m_widget );
		event->accept();
	}
	else if( event->button() == Qt::MiddleButton )
	{
		m_model->reset();
	}
}


void AutomatableModelViewBase::setConversionFactor( float factor )
{
	if( factor != 0.0 )
	{
		m_conversionFactor = factor;
	}
}


float AutomatableModelViewBase::getConversionFactor()
{
	return m_conversionFactor;
}


AutomatableModelViewSlots::AutomatableModelViewSlots( AutomatableModelViewBase* amv, QObject* parent ) :
	QObject(),
	m_amv( amv )
{
	connect( parent, SIGNAL(destroyed()), this, SLOT(deleteLater()), Qt::QueuedConnection );
}




void AutomatableModelViewSlots::execConnectionDialog()
{
	// TODO[pg]: Display a dialog with list of controllers currently in the song
	// in addition to any system MIDI controllers
	auto* m = m_amv->baseModel();

	m->model()->displayName();
	gui::ControllerConnectionDialog d( getGUI()->mainWindow(), m );

	if( d.exec() == 1 )
	{
		// Actually chose something
		if( d.chosenController() )
		{
			// Update
			if( m->controllerConnection() )
			{
				m->controllerConnection()->setController( d.chosenController() );
			}
			// New
			else
			{
				m->setControllerConnection( createControllerconnection(d.chosenController()) );
				//cc->setTargetName( m->displayName() );
			}
		}
		// no controller, so delete existing connection
		else
		{
			removeConnection();
		}
	}
}




void AutomatableModelViewSlots::removeConnection()
{
	auto* m = m_amv->baseModel();

	if( m->controllerConnection() )
	{
		m->setControllerConnection( nullptr );
	}
}




void AutomatableModelViewSlots::editSongGlobalAutomation()
{
	getGUI()->automationEditor()->open(
				SetGlobalAutomationClip(m_amv->baseModel())
	);
}



void AutomatableModelViewSlots::removeSongGlobalAutomation()
{
	delete SetGlobalAutomationClip( m_amv->baseModel() );
}


void AutomatableModelViewSlots::unlinkAllModels()
{
	m_amv->baseModel()->unlinkAllModels();
}

void AutomatableModelViewSlots::copyToClipboard()
{
	// For copyString() and MimeType enum class
	using namespace Clipboard;

	copyString( QString::number( m_amv->baseModel()->valueAsFloat() * m_amv->getConversionFactor() ), MimeType::Default );
}

void AutomatableModelViewSlots::pasteFromClipboard()
{
	bool isNumber = false;
	const float number = floatFromClipboard(&isNumber);
	if (isNumber) {
		m_amv->baseModel()->setValueAsFloat(number / m_amv->getConversionFactor());
	}
}

/// Attempt to parse a float from the clipboard
static float floatFromClipboard(bool* ok)
{
	// For getString() and MimeType enum class
	using namespace Clipboard;

	return getString( MimeType::Default ).toFloat(ok);
}


} // namespace lmms::gui
