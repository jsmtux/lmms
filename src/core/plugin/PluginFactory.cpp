/*
 * PluginFactory.cpp
 *
 * Copyright (c) 2015 Lukas W <lukaswhl/at/gmail.com>
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

#include "PluginFactory.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <memory>
#include "lmmsconfig.h"

#include "IConfigManager.h"
#include "Plugin.h"

// QT qHash specialization, needs to be in global namespace
qint64 qHash(const QFileInfo& fi)
{
	return qHash(fi.absoluteFilePath());
}

namespace lmms
{


IPluginFactory* IPluginFactory::Instance() {
	return PluginFactory::instance();
}

#ifdef LMMS_BUILD_WIN32
	QStringList nameFilters("*.dll");
#else
	#ifdef LMMS_BUILD_APPLE
		QStringList nameFilters("lib*.dylib");
	#else
		QStringList nameFilters("lib*.so");
	#endif
#endif

std::unique_ptr<PluginFactory> PluginFactory::s_instance;

PluginFactory::PluginFactory()
{
	setupSearchPaths();
	discoverPlugins();
}

void PluginFactory::setupSearchPaths()
{
	// Adds a search path relative to the main executable if the path exists.
	auto addRelativeIfExists = [](const QString & path) {
		QDir dir(qApp->applicationDirPath());
		if (!path.isEmpty() && dir.cd(path)) {
			QDir::addSearchPath("plugins", dir.absolutePath());
		}
	};

	// We're either running LMMS installed on an Unixoid or we're running a
	// portable version like we do on Windows.
	// We want to find our plugins in both cases:
	//  (a) Installed (Unix):
	//      e.g. binary at /usr/bin/lmms - plugin dir at /usr/lib/lmms/
	//  (b) Portable:
	//      e.g. binary at "C:/Program Files/LMMS/lmms.exe"
	//           plugins at "C:/Program Files/LMMS/plugins/"

#ifndef LMMS_BUILD_WIN32
	addRelativeIfExists("../lib/lmms"); // Installed
#endif
	addRelativeIfExists("plugins"); // Portable
#ifdef PLUGIN_DIR // We may also have received a relative directory via a define
	addRelativeIfExists(PLUGIN_DIR);
#endif
	// Or via an environment variable:
	QString env_path;
	if (!(env_path = qgetenv("LMMS_PLUGIN_DIR")).isEmpty())
		QDir::addSearchPath("plugins", env_path);

	QDir::addSearchPath("plugins", IConfigManager::Instance()->workingDir() + "plugins");
}

PluginFactory* PluginFactory::instance()
{
	if (s_instance == nullptr)
		s_instance = std::make_unique<PluginFactory>();

	return s_instance.get();
}

PluginFactory* getPluginFactory()
{
	return PluginFactory::instance();
}

PluginDescriptorList PluginFactory::descriptors() const
{
	return m_descriptors.values();
}

PluginDescriptorList PluginFactory::descriptors(PluginTypes type) const
{
	return m_descriptors.values(type);
}

const PluginFactory::PluginInfoList& PluginFactory::pluginInfos() const
{
	return m_pluginInfos;
}

PluginFactory::PluginNameAndKey PluginFactory::pluginSupportingExtension(const QString& ext)
{
	const auto& infoAndKey = m_pluginByExt.value(ext, PluginInfoAndKey());
	return {infoAndKey.info.name(), infoAndKey.key, infoAndKey.info.isNull()};
}

PluginFactory::PluginInfo PluginFactory::pluginInfo(const char* name) const
{
	for (const PluginInfo& info : m_pluginInfos)
	{
		if (qstrcmp(info.descriptor->name, name) == 0)
			return info;
	}
	return PluginInfo();
}

QString PluginFactory::errorString(QString pluginName) const
{
	static QString notfound = qApp->translate("PluginFactory", "Plugin not found.");
	return m_errors.value(pluginName, notfound);
}

void PluginFactory::discoverPlugins()
{
	DescriptorMap descriptors;
	PluginInfoList pluginInfos;
	m_pluginByExt.clear();

	QSet<QFileInfo> files;
	for (const QString& searchPath : QDir::searchPaths("plugins"))
	{
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
		auto discoveredPluginList = QDir(searchPath).entryInfoList(nameFilters);
		qDebug() << "Checking search path " << searchPath << Qt::endl;
		files.unite(QSet<QFileInfo>(discoveredPluginList.begin(), discoveredPluginList.end()));
#else
		files.unite(QDir(searchPath).entryInfoList(nameFilters).toSet());
#endif
	}

	// Cheap dependency handling: zynaddsubfx needs ZynAddSubFxCore. By loading
	// all libraries twice we ensure that libZynAddSubFxCore is found.
	for (const QFileInfo& file : files)
	{
		// qDebug() << "Loading file " << file.fileName() << Qt::endl;
		// QLibrary(file.fileName()).load();
		qDebug() << "Loading file " << file.absoluteFilePath() << Qt::endl;
		QLibrary(file.absoluteFilePath()).load();
	}

	for (const QFileInfo& file : files)
	{
		// auto library = std::make_shared<QLibrary>(file.fileName());
		auto library = std::make_shared<QLibrary>(file.absoluteFilePath());
		library->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
		if (! library->load()) {
			m_errors[file.baseName()] = library->errorString();
			qWarning("%s", library->errorString().toLocal8Bit().data());
			continue;
		}

		PluginDescriptor* pluginDescriptor = nullptr;
		if (library->resolve("lmms_plugin_main"))
		{
			QString libraryCommonName = file.baseName();
			if ( libraryCommonName.contains("_") ) {
				libraryCommonName = libraryCommonName.mid(0, libraryCommonName.indexOf("_"));
			}
			QString descriptorName = libraryCommonName + "_plugin_descriptor";
			if( descriptorName.left(3) == "lib" )
			{
				descriptorName = descriptorName.mid(3);
			}

			pluginDescriptor = reinterpret_cast<PluginDescriptor*>(library->resolve(descriptorName.toUtf8().constData()));
			if(pluginDescriptor == nullptr)
			{
				qWarning() << qApp->translate("PluginFactory", "LMMS plugin %1 does not have a plugin descriptor named %2!").
							  arg(file.absoluteFilePath()).arg(descriptorName);
				continue;
			}
		}

		if(pluginDescriptor)
		{
			PluginInfo info;
			info.file = file;
			info.library = library;
			info.descriptor = pluginDescriptor;
			pluginInfos << info;

			auto addSupportedFileTypes =
				[this](QString supportedFileTypes,
					const PluginInfo& info,
					const PluginDescriptor::Key* key = nullptr)
			{
				if(!supportedFileTypes.isNull())
				{
					for (const QString& ext : supportedFileTypes.split(','))
					{
						// qWarning() << "Plugin " << info.name()
						// 	<< "supports" << ext;
						PluginInfoAndKey infoAndKey;
						infoAndKey.info = info;
						infoAndKey.key = key
							? *key
							: PluginDescriptor::Key();
						m_pluginByExt.insert(ext, infoAndKey);
					}
				}
			};

			if (info.descriptor->supportedFileTypes)
				addSupportedFileTypes(QString(info.descriptor->supportedFileTypes), info);

			if (info.descriptor->subPluginFeatures)
			{
				PluginDescriptor::SubPluginFeatures::KeyList
					subPluginKeys;
				info.descriptor->subPluginFeatures->listSubPluginKeys(
					info.descriptor,
					subPluginKeys);
				for(const PluginDescriptor::Key& key
					: subPluginKeys)
				{
					addSupportedFileTypes(key.additionalFileExtensions(), info, &key);
				}
			}

			descriptors.insert(info.descriptor->type, info.descriptor);
		}
	}

	m_pluginInfos = pluginInfos;
	m_descriptors = descriptors;
}



QString PluginFactory::PluginInfo::name() const
{
	return descriptor ? descriptor->name : QString();
}


} // namespace lmms
