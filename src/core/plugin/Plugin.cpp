/*
 * Plugin.cpp - implementation of plugin-class including plugin-loader
 *
 * Copyright (c) 2005-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "Plugin.h"

#include <iostream>

#include <QtGlobal>
#include <QDomElement>
#include <QLibrary>

#include "IPlugin.h"
#include "IEngine.h"
#include "embed.h"
#include "IGuiApplication.h"
#include "PluginFactory.h"


using lmms::gui::getGUIInterface;

namespace lmms
{


static PixmapLoader dummyLoader;

static PluginDescriptor dummyPluginDescriptor =
{
	"dummy",
	"dummy",
	QT_TRANSLATE_NOOP( "PluginBrowser", "no description" ),
	"Tobias Doerffel <tobydox/at/users.sf.net>",
	0x0100,
	PluginTypes::Undefined,
	&dummyLoader,
	nullptr
} ;




Plugin::Plugin(const PluginDescriptor * descriptor,
		Model * _parent,
		const PluginDescriptor::Key* key) :
	JournallingObject(),
	m_model(_parent),
	m_key(key ? *key : PluginDescriptor::Key(descriptor)),
	m_descriptor(descriptor)
{
	if( m_descriptor == nullptr )
	{
		m_descriptor = &dummyPluginDescriptor;
	}
}



// QString Plugin::displayName() const
// {
// 	return Model::displayName().isEmpty() // currently always empty
// 		? (m_descriptor->subPluginFeatures && m_key.isValid())
// 			// get from sub plugin
// 			? m_key.displayName()
// 			// get from plugin
// 			: m_descriptor->displayName
// 		: Model::displayName();
// }




const PixmapLoader* Plugin::logo() const
{
	return (m_descriptor->subPluginFeatures && m_key.isValid())
		? m_key.logo()
		: m_descriptor->logo;
}



AutomatableModel * Plugin::childModel( const QString & )
{
	// static FloatModel fm;
	// return &fm;
	return nullptr;
}



Plugin * Plugin::instantiateWithKey(const QString& pluginName, Model * parent,
				const PluginDescriptor::Key *key,
				bool keyFromDnd)
{
	if(keyFromDnd)
		Q_ASSERT(!key);
	const PluginDescriptor::Key *keyPtr = keyFromDnd
		? static_cast<PluginDescriptor::Key*>(IEngine::Instance()->pickDndPluginKey())
		: key;
	const PluginFactory::PluginInfo& pi = getPluginFactory()->pluginInfo(pluginName.toUtf8());
	if(keyPtr)
	{
		// descriptor is not yet set when loading - set it now
		PluginDescriptor::Key keyCopy = *keyPtr;
		keyCopy.desc = pi.descriptor;
		return Plugin::instantiate(pluginName, parent, &keyCopy);
	}
	else
		return Plugin::instantiate(pluginName, parent,
			// the keys are never touched anywhere
			const_cast<PluginDescriptor::Key *>(keyPtr));
}




Plugin * Plugin::instantiate(const QString& pluginName, Model * parent,
								void *data)
{
	const PluginFactory::PluginInfo& pi = getPluginFactory()->pluginInfo(pluginName.toUtf8());

	Plugin* inst = nullptr;
	if (gui::getGUIInterface() == nullptr) {
		std::cerr << "Gui implementation missing when trying to create plugin" << std::endl;
	}
	else if( pi.isNull() )
	{
		getGUIInterface()->mainWindowInterface()->ShowInfoMessage(
			QObject::tr( "Plugin not found" ),
			QObject::tr( "The plugin \"%1\" wasn't found or could not be loaded!\nReason: \"%2\"" ).
					arg( pluginName ).arg( getPluginFactory()->errorString(pluginName)) );
	}
	else
	{
		InstantiationHook instantiationHook;
		if ((instantiationHook = ( InstantiationHook ) pi.library->resolve( "lmms_plugin_main" )))
		{
			inst = instantiationHook(parent, data);
		}
		else
		{
			if (gui::getGUIInterface() != nullptr)
			{
				gui::getGUIInterface()->mainWindowInterface()->ShowInfoMessage(
					QObject::tr( "Error while loading plugin" ),
					QObject::tr( "Failed to load plugin \"%1\"!").arg( pluginName ));
			}
		}
	}

	return inst;
}




void Plugin::collectErrorForUI( QString errMsg )
{
	IEngine::Instance()->getSongInterface()->collectError( errMsg );
}


} // namespace lmms
