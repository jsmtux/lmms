/*
 * EffectRackView.h - view for effectChain-model
 *
 * Copyright (c) 2006-2007 Danny McRae <khjklujn@netscape.net>
 * Copyright (c) 2008-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_GUI_EFFECT_RACK_VIEW_H
#define LMMS_GUI_EFFECT_RACK_VIEW_H

#include "EffectChain.h"
#include "lmms_basics.h"

#include <QWidget>

class QScrollArea;
class QVBoxLayout;

namespace lmms::gui
{

class EffectView;
class GroupBox;


class EffectRackView : public QWidget
{
	Q_OBJECT
public:
	EffectRackView( EffectChain* _fxChain, QWidget* parent = nullptr );
	~EffectRackView() override;

	static constexpr int DEFAULT_WIDTH = 245;

public slots:
	void clearViews();
	void moveUp( lmms::gui::EffectView* view );
	void moveDown( lmms::gui::EffectView* view );
	void deletePlugin( lmms::gui::EffectView* view );


private slots:
	virtual void update();
	void addEffect();


private:
	EffectChain* m_fxChain;

	QVector<EffectView *> m_effectViews;

	GroupBox* m_effectsGroupBox;
	QScrollArea* m_scrollArea;

	int m_lastY;

} ;

} // namespace lmms::gui

#endif // LMMS_GUI_EFFECT_RACK_VIEW_H
