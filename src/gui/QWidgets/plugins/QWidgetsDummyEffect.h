/*
 * DummyEffect.h - effect used as fallback if an effect couldn't be loaded
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

#ifndef LMMS_QWIDGETS_DUMMY_EFFECT_H
#define LMMS_QWIDGETS_DUMMY_EFFECT_H

#include <QDomElement>

#include "IPlugin.h"
#include "Effect.h"
#include "EffectControls.h"
#include "DummyEffect.h"

namespace lmms
{

class DummyEffectControls : public EffectControls
{
public:
	DummyEffectControls( Effect * _eff ) :
		EffectControls( _eff )
	{
	}

	~DummyEffectControls() override = default;

	int controlCount() override
	{
		return 0;
	}

	void saveSettings( QDomDocument &, QDomElement & ) override
	{
	}

	void loadSettings( const QDomElement & ) override
	{
	}

	QString nodeName() const override
	{
		return "DummyControls";
	}

	gui::EffectControlDialog * createView() override
	{
		return nullptr;
	}
} ;

class QWidgetsDummyEffect : public DummyEffect {
    Q_OBJECT
public:
    QWidgetsDummyEffect( IEffectChain * _parent, const QDomElement& originalPluginData) :
        DummyEffect(_parent, originalPluginData),
        m_controls(this)
    {}
private:
    DummyEffectControls m_controls;
};



} // namespace lmms

#endif // LMMS_QWIDGETS_DUMMY_EFFECT_H
