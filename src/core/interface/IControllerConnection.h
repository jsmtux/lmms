/*
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


#ifndef LMMS_I_CONTROLLER_CONNECTION_H
#define LMMS_I_CONTROLLER_CONNECTION_H

#include "IController.h"
#include "JournallingObject.h"

namespace lmms
{

class IControllerConnection: public QObject
{
    Q_OBJECT
public:
    virtual ~IControllerConnection() = default;
    virtual IController * getController() = 0;
	virtual void setController( IController * _controller ) = 0;

    // This is used from AutomatableModel... Wrong
    virtual void saveSettingsInPlace( QDomDocument& doc, QDomElement& element ) = 0;
    virtual void loadSettingsInPlace( const QDomElement& element ) = 0;
};

std::unique_ptr<IControllerConnection> createControllerconnection( IController* controller);

}

#endif // LMMS_I_CONTROLLER_CONNECTION_H