/*
 * SendButtonIndicator.h
 *
 * Copyright (c) 2014-2022 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_GUI_SEND_BUTTON_INDICATOR_H
#define LMMS_GUI_SEND_BUTTON_INDICATOR_H

#include <QLabel>

#include "IModels.h"


namespace lmms
{

namespace gui
{

class MixerLine;
class MixerView;


class SendButtonIndicator : public QLabel 
{
public:
	SendButtonIndicator( QWidget * _parent, MixerLine * _owner,
						 MixerView * _mv);

	void mousePressEvent( QMouseEvent * e ) override;
	void updateLightStatus();

private:

	MixerLine * m_parent;
	MixerView * m_mv;
	static QPixmap * s_qpmOn;
	static QPixmap * s_qpmOff;

	IFloatAutomatableModel * getSendModel();
};


} // namespace gui

} // namespace lmms

#endif // LMMS_GUI_SEND_BUTTON_INDICATOR_H
