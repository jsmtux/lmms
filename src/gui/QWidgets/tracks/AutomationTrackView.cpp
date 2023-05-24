/*
 * AutomationTrackView.cpp
 *
 * Copyright (c) 2008-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2006-2008 Javier Serrano Polo <jasp00/at/users.sourceforge.net>
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
 
#include "AutomationTrackView.h"

#include "IClip.h"
#include "ITrack.h"
#include "embed.h"
#include "IEngine.h"
#include "IProjectJournal.h"
#include "StringPairDrag.h"
#include "TrackLabelButton.h"

#include "editors/TrackContainerView.h"

namespace lmms::gui
{

AutomationTrackView::AutomationTrackView( IAutomationTrack * _at, TrackContainerView* tcv ) :
	TrackView( _at->baseTrack(), tcv )
{
        setFixedHeight( 32 );
		auto tlb = new TrackLabelButton(this, getTrackSettingsWidget());
		tlb->setIcon(embed::getIconPixmap("automation_track"));
		tlb->move(3, 1);
		tlb->show();
		QObject::connect( _at->baseTrack(), SIGNAL(dataChanged()), this, SLOT(update()));
		QObject::connect( _at->baseTrack(), SIGNAL(propertiesChanged()), this, SLOT(update()));
}

void AutomationTrackView::dragEnterEvent( QDragEnterEvent * _dee )
{
	StringPairDrag::processDragEnterEvent( _dee, "automatable_model" );
}




void AutomationTrackView::dropEvent( QDropEvent * _de )
{
	QString type = StringPairDrag::decodeKey( _de );
	QString val = StringPairDrag::decodeValue( _de );
	if( type == "automatable_model" )
	{
		auto mod = getAutomatableModelFromJournallingObject(IEngine::Instance()->getProjectJournalInterface()->journallingObject(val.toInt()));
		if( mod != nullptr )
		{
			TimePos pos = TimePos( trackContainerView()->
							currentPosition() +
				( _de->pos().x() -
					getTrackContentWidget()->x() ) *
						TimePos::ticksPerBar() /
		static_cast<int>( trackContainerView()->pixelsPerBar() ) )
				.toAbsoluteBar();

			if( pos.getTicks() < 0 )
			{
				pos.setTicks( 0 );
			}

			auto * clip = m_at->createAutomationClip( pos );
			clip->addObject( mod );
		}
	}

	update();
}


} // namespace lmms::gui
