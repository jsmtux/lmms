/*
 * Plugin.h - class plugin, the base-class and generic interface for all plugins
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

#ifndef LMMS_PLUGIN_H
#define LMMS_PLUGIN_H

#include <QStringList>
#include <QMap>

#include "IMemoryManager.h"
#include "JournallingObject.h"
#include "Model.h"
#include "IPlugin.h"


class QWidget;

namespace lmms
{

class AutomatableModel;
class PixmapLoader;

namespace gui
{

class PluginView;

}

/**
	Abstract representation of a plugin

	Such a plugin can be an Instrument, Effect, Tool plugin etc.

	Plugins have descriptors, containing meta info, which is used especially
	by PluginFactory and friends.

	There are also Plugin keys (class Key, confusingly under
	SubPluginFeatures), which contain pointers to the plugin descriptor.

	Some plugins have sub plugins, e.g. there is one CALF Plugin and for
	each CALF effect, there is a CALF sub plugin. For those plugins, there
	are keys for each sub plugin. These keys also link to the superior
	Plugin::Descriptor. Additionally, they contain attributes that help the
	superior Plugin saving them and recognizing them when loading.

	In case of sub plugins, the Descriptor has SubPluginFeatures. Those
	are a bit like values to the sub plugins' keys (in terms of a key-value-
	map).
*/
class LMMS_EXPORT Plugin : public JournallingObject
{
	I_MM_OPERATORS
public:
	//! Constructor of a plugin
	//! @param key Sub plugins must pass a key here, optional otherwise.
	//!   See the key() function
	Plugin(const PluginDescriptor * descriptor,
		const PluginDescriptor::Key *key = nullptr);
	~Plugin() override = default;

	//! Return display-name out of sub plugin or descriptor
	// QString displayName() const override;

	//! Return logo out of sub plugin or descriptor
	const PixmapLoader *logo() const;

	//! Return plugin type
	inline PluginTypes type() const
	{
		return m_descriptor->type;
	}

	//! Return plugin Descriptor
	inline const PluginDescriptor * descriptor() const
	{
		return m_descriptor;
	}

	//! Return the key referencing this plugin. If the Plugin has no
	//! sub plugin features, the key is pretty useless. If it has,
	//! this key will also contain the sub plugin attributes, and will be
	//! a key to those SubPluginFeatures.
	inline const PluginDescriptor::Key & key() const
	{
		return m_key;
	}

	//! Called if external source needs to change something but we cannot
	//! reference the class header.  Should return null if not key not found.
	virtual AutomatableModel* childModel( const QString & modelName );

	//! Overload if the argument passed to the plugin is a subPluginKey
	//! If you can not pass the key and are aware that it's stored in
	//! Engine::pickDndPluginKey(), use this function, too
	static Plugin * instantiateWithKey(const QString& pluginName, void * parent,
					const PluginDescriptor::Key *key,
					bool keyFromDnd = false);

	//! Return an instance of a plugin whose name matches to given one
	//! if specified plugin couldn't be loaded, it creates a dummy-plugin
	//! @param data Anything the plugin expects. If this is a pointer to a sub plugin key,
	//!   use instantiateWithKey instead
	static Plugin * instantiate(const QString& pluginName, void * parent, void *data);

	//! Create a view for the model

protected:
	void collectErrorForUI( QString errMsg );

	PluginDescriptor::Key m_key;

private:
	const PluginDescriptor * m_descriptor;

	// pointer to instantiation-function in plugin
	using InstantiationHook = Plugin* (*)(void*, void*);
} ;


} // namespace lmms

#endif // LMMS_PLUGIN_H
