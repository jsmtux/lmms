/*
 * Copyright (c) 2005-2023 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_I_PLUGIN_FACTORY_H
#define LMMS_I_PLUGIN_FACTORY_H

#include "IPlugin.h"

namespace lmms
{

class IPluginFactory
{
public:

	struct PluginNameAndKey
	{
		QString name;
		PluginDescriptor::Key key;
		bool isNull;
	};
	/// Returns a list of all found plugins' descriptors.
	virtual PluginDescriptorList descriptors() const = 0;
	virtual PluginDescriptorList descriptors(PluginTypes type) const = 0;
	/// Returns a plugin that support the given file extension
	virtual PluginNameAndKey pluginSupportingExtension(const QString& ext) = 0;
	virtual void setupSearchPaths() = 0;
	static IPluginFactory* Instance();
};

}

#endif // LMMS_I_PLUGIN_FACTORY_H