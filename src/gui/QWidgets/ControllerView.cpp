/*
 * ControllerView.cpp - view-component for an controller
 *
 * Copyright (c) 2008-2009 Paul Giblock <drfaygo/at/gmail.com>
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

#include "ControllerView.h"

#include "embed.h"
#include "GuiApplication.h"
#include "MainWindow.h"
#include "SubWindow.h"

#include "widgets/CaptionMenu.h"

#include "controllerdialogs/ControllerDialog.h"
#include "controllerdialogs/ControllerDialogsFactory.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMdiArea>
#include <QInputDialog>
#include <QVBoxLayout>

namespace lmms::gui
{


ControllerView::ControllerView( Controller * _model, QWidget * _parent ) :
	QFrame( _parent ),
	m_controller(_model),
	m_subWindow( nullptr ),
	m_controllerDlg( nullptr ),
	m_show( true )
{
	this->setFrameStyle( QFrame::StyledPanel );
	this->setFrameShadow( QFrame::Raised );

	auto vBoxLayout = new QVBoxLayout(this);

	auto hBox = new QHBoxLayout();
	vBoxLayout->addLayout(hBox);

	auto label = new QLabel("<b>" + _model->displayName() + "</b>", this);
	QSizePolicy sizePolicy = label->sizePolicy();
	sizePolicy.setHorizontalStretch(1);
	label->setSizePolicy(sizePolicy);

	hBox->addWidget(label);

	auto controlsButton = new QPushButton(tr("Controls"), this);
	connect( controlsButton, SIGNAL(clicked()), SLOT(editControls()));

	hBox->addWidget(controlsButton);

	m_nameLabel = new QLabel(_model->name(), this);
	vBoxLayout->addWidget(m_nameLabel);


	ControllerDialogsFactory factory;
	m_controllerDlg = factory.createControllerDialog(getController(), getGUI()->mainWindow()->workspace());

	m_subWindow = getGUI()->mainWindow()->addWindowedWidget( m_controllerDlg );
	
	Qt::WindowFlags flags = m_subWindow->windowFlags();
	flags &= ~Qt::WindowMaximizeButtonHint;
	m_subWindow->setWindowFlags( flags );
	m_subWindow->setFixedSize( m_subWindow->size() );

	m_subWindow->setWindowIcon( m_controllerDlg->windowIcon() );

	connect( m_controllerDlg, SIGNAL(closed()),
		this, SLOT(closeControls()));

	m_subWindow->hide();

	QObject::connect( m_controller, SIGNAL(dataChanged()), this, SLOT(update()));
	QObject::connect( m_controller, SIGNAL(propertiesChanged()), this, SLOT(update()));

	update();
}




ControllerView::~ControllerView()
{
	if (m_subWindow)
	{
		delete m_subWindow;
	}
}




void ControllerView::editControls()
{
	if( m_show )
	{
		m_subWindow->show();
		m_subWindow->raise();
		m_show = false;
	}
	else
	{
		m_subWindow->hide();
		m_show = true;
	}
}




void ControllerView::closeControls()
{
	m_subWindow->hide();
	m_show = true;
}


void ControllerView::deleteController()
{
	emit( deleteController( this ) );
}

void ControllerView::renameController()
{
	bool ok;
	QString new_name = QInputDialog::getText( this,
			tr( "Rename controller" ),
			tr( "Enter the new name for this controller" ),
			QLineEdit::Normal, m_controller->name() , &ok );
	if( ok && !new_name.isEmpty() )
	{
		m_controller->setName( new_name );
		if( getController()->type() == Controller::LfoController )
		{
			m_controllerDlg->setWindowTitle( tr( "LFO" ) + " (" + new_name + ")" );
		}
		m_nameLabel->setText( new_name );
	}
}


void ControllerView::mouseDoubleClickEvent( QMouseEvent * event )
{
	renameController();
}


void ControllerView::contextMenuEvent( QContextMenuEvent * )
{
	QPointer<CaptionMenu> contextMenu = new CaptionMenu( m_controller->displayName(), this );
	contextMenu->addAction( embed::getIconPixmap( "cancel" ),
						tr( "&Remove this controller" ),
						this, SLOT(deleteController()));
	contextMenu->addAction( tr("Re&name this controller"), this, SLOT(renameController()));
	contextMenu->addSeparator();
	contextMenu->exec( QCursor::pos() );
	delete contextMenu;
}


} // namespace lmms::gui
