/*
 * ControllerView.h - view-component for an control
 *
 * Copyright (c) 2008 Paul Giblock <drfaygo/at/gmail.com>
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

#ifndef LMMS_GUI_CONTROLLER_VIEW_H
#define LMMS_GUI_CONTROLLER_VIEW_H

#include "AutomatableModel.h"
#include "Controller.h"

#include <QFrame>

class QGroupBox;
class QLabel;
class QPushButton;
class QMdiSubWindow;

namespace lmms::gui
{


class LedCheckBox;


class ControllerView : public QFrame
{
	Q_OBJECT
public:
	ControllerView( Controller * _controller, QWidget * _parent );
	~ControllerView() override;

	inline Controller * getController()
	{
		return m_controller;
	}

	inline const Controller * getController() const
	{
		return m_controller;
	}


public slots:
	void editControls();
	void deleteController();
	void closeControls();
	void renameController();

signals:
	void deleteController( lmms::gui::ControllerView * _view );


protected:
	void contextMenuEvent( QContextMenuEvent * _me ) override;
	void mouseDoubleClickEvent( QMouseEvent * event ) override;


private:
	Controller* m_controller;
	QMdiSubWindow * m_subWindow;
	ControllerDialog * m_controllerDlg;
	QLabel * m_nameLabel;
	bool m_show;

} ;


} // namespace lmms::gui

#endif // LMMS_GUI_CONTROLLER_VIEW_H
