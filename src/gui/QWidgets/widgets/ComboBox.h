/*
 * ComboBox.h - class ComboBox, a combo box view for models
 *
 * Copyright (c) 2006-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_GUI_COMBOBOX_H
#define LMMS_GUI_COMBOBOX_H

#include "AutomatableModelView.h"
#include "IModels.h"

#include <QMenu>
#include <QWidget>

namespace lmms::gui
{

class LMMS_EXPORT ComboBox : public QWidget, public IntModelView
{
	Q_OBJECT
public:
	ComboBox( IComboBoxModelWrapper* _model, QWidget* parent = nullptr, const QString& name = QString() );
	~ComboBox() override = default;

	IComboBoxModelWrapper* comboboxModel() {
		return m_comboboxModel;
	}
	// ComboBoxModel* model()
	// {
	// 	return static_cast<ComboBoxModel*>(m_model);
	// }

	// const ComboBoxModel* model() const
	// {
	// 	return static_cast<ComboBoxModel*>(m_model);
	// }

	static constexpr int DEFAULT_HEIGHT = 22;

public slots:
	void selectNext();
	void selectPrevious();


protected:
	void contextMenuEvent( QContextMenuEvent* event ) override;
	void mousePressEvent( QMouseEvent* event ) override;
	void paintEvent( QPaintEvent* event ) override;
	void wheelEvent( QWheelEvent* event ) override;


private:
	static QPixmap* s_background;
	static QPixmap* s_arrow;
	static QPixmap* s_arrowSelected;

	QMenu m_menu;

	bool m_pressed;
	IComboBoxModelWrapper* m_comboboxModel;


private slots:
	void setItem( QAction* item );

} ;

} // namespace lmms::gui

#endif // LMMS_GUI_COMBOBOX_H