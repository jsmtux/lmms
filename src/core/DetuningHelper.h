/*
 * DetuningHelper.h - detuning automation helper
 *
 * Copyright (c) 2007 Javier Serrano Polo <jasp00/at/users.sourceforge.net>
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

#ifndef LMMS_DETUNING_HELPER_H
#define LMMS_DETUNING_HELPER_H

#include "tracks/AutomationTrack.h"
#include "InlineAutomation.h"
#include "MemoryManager.h"
#include "IDetuningHelper.h"

namespace lmms
{

class DetuningHelper : public IDetuningHelper, public InlineAutomation
{
	Q_OBJECT
	MM_OPERATORS
public:
	DetuningHelper() :
		InlineAutomation()
	{
		m_model.setNodeName("detuning");
		m_model.setDisplayName(tr( "Note detuning" ));
	}

	~DetuningHelper() override = default;

	float defaultValue() const override
	{
		return 0;
	}

	void setRange( const float min, const float max, const float step = 1 ) override {
		m_model.setRange(min, max, step);
	}

	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override {
		return InlineAutomation::saveSettings(_doc, _parent);
	}
	void loadSettings( const QDomElement & _this ) override {
		return InlineAutomation::loadSettings(_this);
	}

    IAutomationClip * automationClip() override {
		return InlineAutomation::automationClip();
	}

	bool hasAutomation() const override {
		return InlineAutomation::hasAutomation();
	}
} ;

} // namespace lmms

#endif // LMMS_DETUNING_HELPER_H
