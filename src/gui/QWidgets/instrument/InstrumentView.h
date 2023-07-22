/*
 * InstrumentView.h - definition of InstrumentView-class
 *
 * Copyright (c) 2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_GUI_INSTRUMENT_VIEW_H
#define LMMS_GUI_INSTRUMENT_VIEW_H

#include "Instrument.h"
#include "PluginView.h"


namespace lmms::gui
{

class InstrumentTrackWindow;


//! Instrument view with variable size
class LMMS_EXPORT InstrumentView : public PluginView
{
public:
	InstrumentView( Instrument * _instrument, QWidget * _parent );
	~InstrumentView() override;

	InstrumentTrackWindow * instrumentTrackWindow();
} ;

template<typename InstrumentType>
class LMMS_EXPORT InstrumentViewImpl : public InstrumentView
{
public:
	InstrumentViewImpl( InstrumentType * _instrument, QWidget * _parent, bool _is_fixed_size ) :
		InstrumentView(_instrument, _parent),
		is_fixed_size(_is_fixed_size)
	{
		m_instrument = _instrument;
	}
	~InstrumentViewImpl() override = default;
protected:
	QSize sizeHint() const override {
		if (is_fixed_size){
			return QSize(250, 250);
		} else {
			return QWidget::sizeHint();
		}
	}

	QSize minimumSizeHint() const override {
		if (is_fixed_size){
			return sizeHint();
		} else {
			return QWidget::minimumSizeHint();
		}		
	}

	InstrumentType* m_instrument;
	bool is_fixed_size;
};

} // namespace lmms::gui

#endif // LMMS_GUI_INSTRUMENT_VIEW_H
