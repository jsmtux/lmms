/*
 * TripleOscillator.h - declaration of class TripleOscillator a powerful
 *                      instrument-plugin with 3 oscillators
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _TRIPLE_OSCILLATOR_QWIDGETS_H
#define _TRIPLE_OSCILLATOR_QWIDGETS_H

#include "IModels.h"

#include "instrument/InstrumentView.h"

#include "plugins/QWidgetInstrumentPlugin.h"
#include "TripleOscillator.h"

namespace lmms
{


class NotePlayHandle;
class SampleBuffer;
class Oscillator;


namespace gui
{
class automatableButtonGroup;
class Knob;
class PixmapButton;
class TripleOscillatorView;



class QWidgetTripleOscillator : public TripleOscillator, public IQWidgetInstrumentPlugin
{
	Q_OBJECT
public:
	QWidgetTripleOscillator( IInstrumentTrack * _track ) :
        TripleOscillator(_track) {}

	gui::InstrumentView* createView( QWidget * _parent ) override;
	friend class gui::TripleOscillatorView;

} ;


class TripleOscillatorView : public InstrumentViewImpl<TripleOscillator>
{
	Q_OBJECT
public:
	TripleOscillatorView( TripleOscillator * _instrument, QWidget * _parent );
	~TripleOscillatorView() override = default;


private:
	automatableButtonGroup * m_mod1BtnGrp;
	automatableButtonGroup * m_mod2BtnGrp;
} ;


} // namespace gui

} // namespace lmms

#endif
