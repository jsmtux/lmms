/*
 * ControllerConnectionDialog.h - dialog allowing the user to create and
 *	modify links between controllers and models
 *
 * Copyright (c) 2008  Paul Giblock <drfaygo/at/gmail.com>
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

#ifndef LMMS_GUI_CONTROLLER_CONNECTION_DIALOG_H
#define LMMS_GUI_CONTROLLER_CONNECTION_DIALOG_H

#include "IController.h"
#include "IModels.h"
#include "IMidiController.h"

#include <QDialog>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

class QLineEdit;
class QListView;
class QScrollArea;

namespace lmms
{

class IAutoDetectMidiController;

namespace gui
{

class ComboBox;
class GroupBox;
class TabWidget;
class LcdSpinBox;
class LedCheckBox;
class MidiPortMenu;


class ControllerConnectionDialog : public QDialog
{
	Q_OBJECT
public:
	ControllerConnectionDialog( QWidget * _parent,
			const IAutomatableModelBase* _target_model );
	~ControllerConnectionDialog() override;

	IController * chosenController()
	{
		return m_controller;
	}

public slots:
//	void setSelection( const effectKey & _selection );
	void selectController();
	void midiToggled();
	void userToggled();
	void userSelected();
	void autoDetectToggled();
	void enableAutoDetect( QAction * _a );


protected slots:
	void midiValueChanged();


private:
	// Midi
	GroupBox * m_midiGroupBox;
	LcdSpinBox * m_midiChannelSpinBox;
	LcdSpinBox * m_midiControllerSpinBox;
	LedCheckBox * m_midiAutoDetectCheckBox;
	MidiPortMenu * m_readablePorts;
	std::unique_ptr<IBoolAutomatableModel> m_midiAutoDetect;

	// User
	GroupBox * m_userGroupBox;
	ComboBox * m_userController;

	// Mapping
	TabWidget * m_mappingBox;
	QLineEdit * m_mappingFunction;

	IController * m_controller;
	const IAutomatableModelBase* m_targetModel;

	// Temporary midiController
	std::unique_ptr<IMidiController> m_midiController;
} ;


} // namespace gui

} // namespace lmms

#endif // LMMS_GUI_CONTROLLER_CONNECTION_DIALOG_H
