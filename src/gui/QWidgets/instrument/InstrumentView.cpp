/*
 * InstrumentView.cpp - base-class for views of all Instruments
 *
 * Copyright (c) 2008-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "embed.h"
#include "InstrumentTrack.h"
#include "InstrumentTrackWindow.h"
#include "InstrumentView.h"

#include <QIcon>

namespace lmms::gui
{

InstrumentView::InstrumentView( Instrument * _instrument, QWidget * _parent ) :
	PluginView( _parent )
{
	instrumentTrackWindow()->setWindowIcon( _instrument->logo()->pixmap() );
	connect( _instrument, SIGNAL(destroyed(QObject*)), this, SLOT(close()));
	// connect( _instrument->model(), &Model::dataChanged, this, &QWidget::update);
	// connect( _instrument->model(), &Model::propertiesChanged, this, &QWidget::update);
	setAttribute( Qt::WA_DeleteOnClose, true );
}




InstrumentView::~InstrumentView()
{
	if( instrumentTrackWindow() )
	{
		instrumentTrackWindow()->m_instrumentView = nullptr;
	}
}




InstrumentTrackWindow * InstrumentView::instrumentTrackWindow()
{
	return( dynamic_cast<InstrumentTrackWindow *>(
					parentWidget()->parentWidget() ) );
}





} // namespace lmms::gui
