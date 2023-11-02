/*
 * InlineAutomation.h - class for automating something "inline"
 *
 * Copyright (c) 2008-2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_INLINE_AUTOMATION_H
#define LMMS_INLINE_AUTOMATION_H

#include "AutomationNode.h"
#include "AutomationClip.h"

namespace lmms
{

class InlineAutomation
{
public:
	InlineAutomation() :
		m_autoClip( nullptr )
	{
	}

	~InlineAutomation()
	{
		if( m_autoClip )
		{
			delete m_autoClip;
		}
	}

	virtual float defaultValue() const = 0;

	bool hasAutomation() const
	{
		if( m_autoClip != nullptr && m_autoClip->getTimeMap().isEmpty() == false )
		{
			// Prevent saving inline automation if there's just one node at the beginning of
			// the clip, which has a InValue equal to the value of model (which is going
			// to be saved anyways) and no offset between the InValue and OutValue
			AutomationTimeMap::const_iterator firstNode =
				m_autoClip->getTimeMap().begin();

			if (m_model.isAtInitValue()
				&& m_autoClip->getTimeMap().size() == 1
				&& POS(firstNode) == 0
				&& INVAL(firstNode) == m_model.value()
				&& OFFSET(firstNode) == 0)
			{
				return false;
			}

			return true;
		}

		return false;
	}

	AutomationClip * automationClip()
	{
		if( m_autoClip == nullptr )
		{
			m_autoClip = new AutomationClip( nullptr );
			m_autoClip->addObject( &m_model );
		}
		return m_autoClip;
	}

	void saveSettings( QDomDocument & _doc, QDomElement & _parent );
	void loadSettings( const QDomElement & _this );

protected:
	FloatModel m_model;

private:
	AutomationClip * m_autoClip;
} ;


} // namespace lmms

#endif // LMMS_INLINE_AUTOMATION_H