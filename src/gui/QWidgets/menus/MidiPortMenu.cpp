/*
 * MidiPortMenu.cpp - a menu for subscribing a MidiPort to several external
 *                      MIDI ports
 *
 * Copyright (c) 2008-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "MidiPortMenu.h"

#include "gui_templates.h"

namespace lmms::gui
{


MidiPortMenu::MidiPortMenu( IMidiPort::Modes _mode, IMidiPort* _midiPort ) :
	m_mode( _mode ),
	m_midiPort(_midiPort)
{
	setFont( pointSize<9>( font() ) );
	connect( this, SIGNAL(triggered(QAction*)),
			this, SLOT(activatedPort(QAction*)));
	if( m_mode == IMidiPort::Input )
	{
		connect( m_midiPort->midiPortModel(), SIGNAL(readablePortsChanged()),
				this, SLOT(updateMenu()));
	}
	else if( m_mode == IMidiPort::Output )
	{
		connect( m_midiPort->midiPortModel(), SIGNAL(writablePortsChanged()),
				this, SLOT(updateMenu()));
	}
	updateMenu();
	
	QObject::connect( m_midiPort->model(), SIGNAL(dataChanged()), this, SLOT(update()));
	QObject::connect( m_midiPort->model(), SIGNAL(propertiesChanged()), this, SLOT(update()));
	update();
}



void MidiPortMenu::activatedPort( QAction * _item )
{
	if( m_mode == IMidiPort::Input )
	{
		m_midiPort->subscribeReadablePort( _item->text(),
							_item->isChecked() );
	}
	else if( m_mode == IMidiPort::Output )
	{
		m_midiPort->subscribeWritablePort( _item->text(),
							_item->isChecked() );
	}
}




void MidiPortMenu::updateMenu()
{
	const IMidiPort::Map & map = ( m_mode == IMidiPort::Input ) ?
				m_midiPort->readablePorts() : m_midiPort->writablePorts();
	clear();
	for( IMidiPort::Map::ConstIterator it = map.begin();
							it != map.end(); ++it )
	{
		QAction * a = addAction( it.key() );
		a->setCheckable( true );
		a->setChecked( it.value() );
	}
}


} // namespace lmms::gui


