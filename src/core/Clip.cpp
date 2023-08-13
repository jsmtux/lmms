/*
 * Clip.cpp - implementation of Clip class
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

#include "Clip.h"

#include <QDomDocument>

#include "AutomationClip.h"
#include "Engine.h"
#include "IAutomationEditor.h"
#include "IGuiApplication.h"
#include "Song.h"


namespace lmms
{

QString ClipTypeToString(const ClipType _type) {
	switch(_type) {
		case ClipType::Automation:
			return "Automation";
		case ClipType::Midi:
			return "Midi";
		case ClipType::Pattern:
			return "Pattern";
		case ClipType::Sample:
			return "Sample";
	}
}

std::size_t ClipTypeToId(const ClipType _type) {
	static_cast<std::size_t>(_type);
}

/*! \brief Move this Clip's position in time
 *
 *  If the clip has moved, update its position.  We
 *  also add a journal entry for undo and update the display.
 *
 * \param _pos The new position of the clip.
 */
void Clip::movePosition( const TimePos & pos )
{
	TimePos newPos = qMax(0, pos.getTicks());
	if (m_startPosition != newPos)
	{
		Engine::audioEngine()->requestChangeInModel();
		m_startPosition = newPos;
		Engine::audioEngine()->doneChangeInModel();
		Engine::getSong()->updateLength();
		emit positionChanged();
	}
}




/*! \brief Change the length of this Clip
 *
 *  If the clip's length has changed, update it.  We
 *  also add a journal entry for undo and update the display.
 *
 * \param _length The new length of the clip.
 */
void Clip::changeLength( const TimePos & length )
{
	m_length = length;
	Engine::getSong()->updateLength();
	emit lengthChanged();
}




bool Clip::comparePosition(const Clip *a, const Clip *b)
{
	return a->startPosition() < b->startPosition();
}




/*! \brief Copies the state of a Clip to another Clip
 *
 *  This method copies the state of a Clip to another Clip
 */
void Clip::copyStateTo( Clip *src, Clip *dst )
{
	// If the node names match we copy the state
	if( src->nodeName() == dst->nodeName() ){
		QDomDocument doc;
		QDomElement parent = doc.createElement( "StateCopy" );
		src->saveState( doc, parent );

		const TimePos pos = dst->startPosition();
		dst->restoreState( parent.firstChild().toElement() );
		dst->movePosition( pos );

		AutomationClip::resolveAllIDs();
		gui::getGUIInterface()->automationEditorInterface()->updateAfterClipChange();
	}
}




/*! \brief Mutes this Clip
 *
 *  Restore the previous state of this clip. This will
 *  restore the position or the length of the clip
 *  depending on what was changed.
 *
 * \param _je The journal entry to undo
 */
void Clip::toggleMute()
{
	m_mutedModel.setValue( !m_mutedModel.value() );
	emit m_clipModel.dataChanged();
}




TimePos Clip::startTimeOffset() const
{
	return m_startTimeOffset;
}




void Clip::setStartTimeOffset( const TimePos &startTimeOffset )
{
	m_startTimeOffset = startTimeOffset;
}



void Clip::useCustomClipColor( bool b )
{
	if (b == m_useCustomClipColor) { return; }
	m_useCustomClipColor = b;
	emit colorChanged();
}


bool Clip::hasColor()
{
	return usesCustomClipColor() || trackUseColor();
}

} // namespace lmms
