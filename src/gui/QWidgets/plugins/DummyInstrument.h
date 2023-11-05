/*
 * DummyInstrument.h - instrument used as fallback if an instrument couldn't
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

#include "instrument/InstrumentView.h"

#include <cstring>

namespace lmms
{


class DummyInstrument : public gui::QWidgetInstrumentPlugin
{
public:
	DummyInstrument( IInstrumentTrack * _instrument_track ) :
		gui::QWidgetInstrumentPlugin( _instrument_track, nullptr )
	{
	}

	~DummyInstrument() override = default;

	void playNote( INotePlayHandle *, sampleFrame * buffer ) override
	{
		memset( buffer, 0, sizeof( sampleFrame ) *
			IEngine::Instance()->getAudioEngineInterface()->framesPerPeriod() );
	}

	void saveSettings( QDomDocument &, QDomElement & ) override
	{
	}

	void loadSettings( const QDomElement & ) override
	{
	}

	QString nodeName() const override
	{
		return "dummyinstrument";
	}

	gui::InstrumentView * instantiateView( QWidget * _parent ) override
	{
		return new gui::InstrumentViewImpl<DummyInstrument>( this, _parent, true );
	}
} ;


} // namespace lmms

#endif // LMMS_GUI_DUMMY_INSTRUMENT_H
