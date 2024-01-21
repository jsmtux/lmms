/*
 * ImportFilter.cpp - base-class for all import-filters
 *
 * Copyright (c) 2006-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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


#include <memory>

#include "ImportFilter.h"
#include "IEngine.h"
#include "IProjectJournal.h"
#include "IGuiApplication.h"
#include "PluginFactory.h"

using lmms::gui::getGUIInterface;

namespace lmms
{

using std::unique_ptr;

void importFilterImport( const QString & _file_to_import, ITrackContainer* tc ) {
	ImportFilter::import(_file_to_import, tc);
}

ImportFilter::ImportFilter( const QString & _file_name,
							const PluginDescriptor * _descriptor ) :
	Plugin( _descriptor, nullptr ),
	m_file( _file_name )
{
}





void ImportFilter::import( const QString & _file_to_import,
							ITrackContainer* tc )
{
	bool successful = false;

	QByteArray s = _file_to_import.toUtf8();
	s.detach();

	// do not record changes while importing files
	const bool j = IEngine::Instance()->getProjectJournalInterface()->isJournalling();
	IEngine::Instance()->getProjectJournalInterface()->setJournalling( false );

	for (const PluginDescriptor* desc : getPluginFactory()->descriptors(PluginTypes::ImportFilter))
	{
		unique_ptr<Plugin> p(Plugin::instantiate( desc->name, nullptr, s.data() ));
		if( dynamic_cast<ImportFilter *>( p.get() ) != nullptr &&
			dynamic_cast<ImportFilter *>( p.get() )->tryImport( tc ) )
		{
			successful = true;
			break;
		}
	}

	IEngine::Instance()->getProjectJournalInterface()->setJournalling( j );

	if( successful == false )
	{
		getGUIInterface()->mainWindowInterface()->ShowInfoMessage(
			QObject::tr( "Couldn't import file" ),
			QObject::tr( "Couldn't find a filter for "
						"importing file %1.\n"
						"You should convert this file "
						"into a format supported by "
						"LMMS using another software."
						).arg( _file_to_import )
		);
	}
}




bool ImportFilter::openFile()
{
	if( m_file.open( QFile::ReadOnly ) == false )
	{
		getGUIInterface()->mainWindowInterface()->ShowCriticalMessage(
			QObject::tr( "Couldn't open file" ),
			QObject::tr( "Couldn't open file %1 "
						"for reading.\nPlease make "
						"sure you have read-"
						"permission to the file and "
						"the directory containing the "
						"file and try again!" ).arg(
							m_file.fileName() ));
		return false;
	}
	return true;
}



} // namespace lmms