/*
 * Kicker.h - drum synthesizer
 *
 * Copyright (c) 2006-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2014 grejppi <grejppi/at/gmail.com>
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


#ifndef TRIPLE_OSCILLATOR_GUI_H
#define TRIPLE_OSCILLATOR_GUI_H

#include "TripleOscillator.h"


namespace lmms
{

class TripleOscillatorQmlInstrument : public TripleOscillator {
    Q_OBJECT
public:
    TripleOscillatorQmlInstrument(IInstrumentTrack * _instrument_track)
		: TripleOscillator(_instrument_track)
	{}
};

} // namespace lmms

#endif
