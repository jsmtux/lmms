/*
 * Plugin.h - class plugin, the base-class and generic interface for all plugins
 *
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

#ifndef LMMS_PLUGIN_DESCRIPTOR_H
#define LMMS_PLUGIN_DESCRIPTOR_H

#include <QString>
#include <QMap>
#include "embed.h"

namespace lmms
{


enum class PluginTypes
{
    Instrument,	// instrument being used in channel-track
    Effect,		// effect-plugin for effect-board
    ImportFilter,	// filter for importing a file
    ExportFilter,	// filter for exporting a file
    Tool,		// additional tool (level-meter etc)
    Library,	// simple library holding a code-base for
            // several other plugins (e.g. VST-support)
    Other,
    Undefined = 255
} ;

namespace {
template<class T>
T use_this_or(T this_param, T or_param)
{
	return this_param ? this_param : or_param;
}


QString use_this_or(QString this_param, QString or_param)
{
	return this_param.isNull() ? or_param : this_param;
}

} // namespace

//! Descriptor holds information about a plugin - every external plugin
//! has to instantiate such a Descriptor in an extern "C"-section so that
//! the plugin-loader is able to access information about the plugin
struct PluginDescriptor
{
    const char * name;
    const char * displayName;
    const char * description;
    const char * author;
    int version;
    PluginTypes type;
    const PixmapLoader * logo;
    const char * supportedFileTypes; //!< csv list of extensions

    inline bool supportsFileType( const QString& extension ) const
    {
        return QString( supportedFileTypes ).split( QChar( ',' ) ).contains( extension );
    }
    
    /**
        Key reference a PluginDescriptor, and,
        if the plugin has sub plugins, also reference
        its sub plugin (using the attributes).
        When keys are saved, those attributes are
        written to XML in order to find the right sub
        plugin when realoading.

        @note Any data that is not required to reference
            the right Plugin or sub plugin should
            not be here (but rather in
            SubPluginFeatures, which are like values
            in a key-value map).
    */
    struct Key
    {
        using AttributeMap = QMap<QString, QString>;

        inline Key( const PluginDescriptor * desc = nullptr,
                const QString & name = QString(),
                const AttributeMap & am = AttributeMap())
            :
            desc( desc ),
            name( name ),
            attributes( am )
        {
        }

        Key( const QDomElement & key )  :
            desc( nullptr ),
            name( key.attribute( "key" ) ),
            attributes()
        {
            QDomNodeList l = key.elementsByTagName( "attribute" );
            for( int i = 0; !l.item( i ).isNull(); ++i )
            {
                QDomElement e = l.item( i ).toElement();
                attributes[e.attribute( "name" )] = e.attribute( "value" );
            }
                
        }

        QDomElement saveXML( QDomDocument & doc ) const
        {
            QDomElement e = doc.createElement( "key" );
            for( AttributeMap::ConstIterator it = attributes.begin(); 
                it != attributes.end(); ++it )
            {
                QDomElement a = doc.createElement( "attribute" );
                a.setAttribute( "name", it.key() );
                a.setAttribute( "value", it.value() );
                e.appendChild( a );
            }
            return e;
        }

        inline bool isValid() const
        {
            return desc != nullptr;
        }

        //! Key to subplugin: reference to parent descriptor
        //! Key to plugin: reference to its descriptor
        const PluginDescriptor* desc;
        //! Descriptive name like "Calf Phaser".
        //! Not required for key lookup and not saved
        //! only used sometimes to temporary store descriptive names
        //! @todo This is a bug, there should be a function
        //!   in SubPluginFeatures (to get the name) instead
        QString name;
        //! Attributes that make up the key and identify
        //! the sub plugin. They are being loaded and saved
        AttributeMap attributes;

        // helper functions to retrieve data that is
        // not part of the key, but mapped via desc->subPluginFeatures
        QString additionalFileExtensions() const
        {
            Q_ASSERT(isValid());
            return desc->subPluginFeatures
                // get from sub plugin
                ? desc->subPluginFeatures->additionalFileExtensions(*this)
                // no sub plugin, so no *additional* file extensions
                : QString();
        }

        QString displayName() const
        {
            Q_ASSERT(isValid());
            return desc->subPluginFeatures
                // get from sub plugin
                ? use_this_or(desc->subPluginFeatures->displayName(*this),
                    QString::fromUtf8(desc->displayName))
                // get from plugin
                : desc->displayName;
        }

        QString description() const
        {
            Q_ASSERT(isValid());
            return desc->subPluginFeatures
                ? use_this_or(desc->subPluginFeatures->description(*this),
                    QString::fromUtf8(desc->description))
                : desc->description;
        }

        const PixmapLoader* logo() const
        {
            Q_ASSERT(isValid());
            return desc->subPluginFeatures
                ? use_this_or(desc->subPluginFeatures->logo(*this), desc->logo)
                : desc->logo;
        }
    };

    /**
        Access to non-key-data of a sub plugin

        If you consider sub plugin keys as keys in a
        key-value-map, this is the lookup for the corresponding
        values. In order to have flexibility between different
        plugin APIs, this is rather an array of fixed data,
        but a bunch of virtual functions taking the key and
        returning some values (or modifying objects of other
        classes).
        */
    class LMMS_EXPORT SubPluginFeatures
    {
    public:
        using KeyList = QList<Key>;

        SubPluginFeatures( PluginTypes type ) :
            m_type( type )
        {
        }

        virtual ~SubPluginFeatures() = default;

        virtual void fillDescriptionWidget( QWidget *, const Key * ) const
        {
        }

        //! While PluginFactory only collects the plugins,
        //! this function is used by widgets like EffectSelectDialog
        //! to find all possible sub plugins
        virtual void listSubPluginKeys( const PluginDescriptor *, KeyList & ) const
        {
        }


    private:
        // You can add values mapped by "Key" below
        // The defaults are sane, i.e. redirect to sub plugin's
        // supererior descriptor

        virtual QString additionalFileExtensions(const Key&) const
        {
            return QString();
        }

        virtual QString displayName(const Key& k) const
        {
            return k.isValid() ? k.name : QString();
        }

        virtual QString description(const Key& k) const
        {
            return k.isValid() ? k.desc->description : QString();
        }

        virtual const PixmapLoader* logo(const Key& k) const
        {
            Q_ASSERT(k.desc);
            return k.desc->logo;
        }

    protected:
        const PluginTypes m_type;
        friend Key;
    } ;

    SubPluginFeatures * subPluginFeatures;
};

// typedef a list so we can easily work with list of plugin descriptors
using PluginDescriptorList = QList<PluginDescriptor*>;

} // namespace lmms

#endif // LMMS_PLUGIN_DESCRIPTOR_H