/*
 * QWidgetDummyInstrument.h - instrument used as fallback if an instrument couldn't
 *                     be loaded
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

#ifndef LMMS_GUI_DUMMY_INSTRUMENT_H
#define LMMS_GUI_DUMMY_INSTRUMENT_H

#include "IAudioEngine.h"
#include "IEngine.h"
#include "QWidgetInstrumentPlugin.h"
#include "DummyInstrument.h"

#include "instrument/InstrumentView.h"

#include <cstring>

namespace lmms
{


class QWidgetDummyInstrument : public DummyInstrument, public IQWidgetInstrumentPlugin
{
public:
	QWidgetDummyInstrument( IInstrumentTrack * _instrument_track ) :
		DummyInstrument(_instrument_track)
	{
	}


    //! Create a view for the model
	gui::PluginView * createView( QWidget * parent ) override {
		return new gui::InstrumentViewImpl<QWidgetDummyInstrument>( this, parent, true );
    }
} ;


} // namespace lmms

#endif // LMMS_GUI_DUMMY_INSTRUMENT_H
