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

#ifndef LMMS_DUMMY_EFFECT_H
#define LMMS_DUMMY_EFFECT_H

#include <QDomElement>

#include "IPlugin.h"
#include "Effect.h"

namespace lmms
{

class DummyEffect : public Effect, public IDummyEffect
{
	Q_OBJECT
public:
	DummyEffect( IEffectChain * _parent, const QDomElement& originalPluginData) :
		Effect( nullptr, _parent, nullptr ),
		m_originalPluginData( originalPluginData )
	{
		setName();
	}

	~DummyEffect() override = default;

	EffectControls * controls() override
	{
		return m_controls;
	}

	bool processAudioBuffer( sampleFrame *, const fpp_t ) override
	{
		return false;
	}

	QDomElement saveState( QDomDocument & _doc, QDomElement & _parent ) override
	{
		_parent.appendChild(originalPluginData());
		return QDomElement();
	}

	const QDomElement& originalPluginData() const
	{
		return m_originalPluginData;
	}

	bool isDummy() override
	{
		return true;
	}


	const PluginDescriptor* descriptor() const override {return nullptr;}
	const PluginDescriptor::Key& key() override {return m_key;}
	void restoreState(const QDomElement&) override {}

private:
	EffectControls* m_controls;
	const QDomElement m_originalPluginData;
	PluginDescriptor::Key m_key;
	
	// Parse the display name from the dom
	virtual void setName()
	{
		QDomNodeList keys = originalPluginData().elementsByTagName( "key" );
		for( int i = 0; !keys.item( i ).isNull(); ++i )
		{
			QDomNodeList attributes = keys.item( i ).toElement().elementsByTagName( "attribute" );
			for( int j = 0; !attributes.item( j ).isNull(); ++j )
			{
				QDomElement attribute = attributes.item( j ).toElement();
				if( attribute.hasAttribute( "value" ) )
				{
					QString name = tr("NOT FOUND") + " (" + attribute.attribute( "value" ) + ")";
					// setDisplayName(name);
					return;
				}

			}

		}
	}
} ;


} // namespace lmms

#endif // LMMS_DUMMY_EFFECT_H
