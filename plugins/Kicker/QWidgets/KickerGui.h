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


#ifndef KICKER_QEIDGETS_H
#define KICKER_QEIDGETS_H

#include "IModels.h"

#include "instrument/InstrumentView.h"
#include "plugins/QWidgetInstrumentPlugin.h"
#include "Kicker.h"


namespace lmms
{

#define KICKER_PRESET_VERSION 1


class NotePlayHandle;

namespace gui
{
class Knob;
class LedCheckBox;
class KickerInstrumentView;
}

namespace gui
{

class KickerQWidgetIntrument : public KickerInstrument, public IQWidgetInstrumentPlugin
{
public:
	KickerQWidgetIntrument(IInstrumentTrack * _instrument_track)
		: KickerInstrument(_instrument_track)
	{}
	gui::PluginView * createView( QWidget * parent );
};

class KickerInstrumentView : public InstrumentViewImpl<KickerInstrument>
{
	Q_OBJECT
public:
	KickerInstrumentView( KickerInstrument * _instrument, QWidget * _parent );
	~KickerInstrumentView() override = default;

private:
	Knob * m_startFreqKnob;
	Knob * m_endFreqKnob;
	Knob * m_decayKnob;
	Knob * m_distKnob;
	Knob * m_distEndKnob;
	Knob * m_gainKnob;
	Knob * m_envKnob;
	Knob * m_noiseKnob;
	Knob * m_clickKnob;
	Knob * m_slopeKnob;

	LedCheckBox * m_startNoteToggle;
	LedCheckBox * m_endNoteToggle;

} ;


} // namespace gui

} // namespace lmms

#endif
