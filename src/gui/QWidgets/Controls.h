/*
 * Controls.h - labeled control widgets
 *
 * Copyright (c) 2019-2019 Johannes Lorenz <j.git$$$lorenz-ho.me, $$$=@>
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

#ifndef LMMS_GUI_CONTROLS_H
#define LMMS_GUI_CONTROLS_H

// headers only required for covariance
#include "IModels.h"
#include "AutomatableModelView.h"


class QString;
class QWidget;
class QLabel;

namespace lmms
{

namespace gui
{

class Knob;
class ComboBox;
class LedCheckBox;

class IControl {
public:
	virtual QWidget* topWidget() = 0;
	virtual void setText(const QString& text) = 0;
};

/**
	These classes provide
	- a control with a text label
	- a type safe way to set a model
		(justification: setting the wrong typed model to a widget will cause
		hard-to-find runtime errors)
*/
template <typename ModelType>
class Control : public IControl
{
public:
	virtual AutomatableModelView<ModelType>* modelView() = 0;

	virtual ~Control() = default;
};


class KnobControl : public Control<float>
{
	Knob* m_knob;

public:
	void setText(const QString& text) override;
	QWidget* topWidget() override;

	FloatModelView* modelView() override;

	KnobControl(IFloatAutomatableModel* _model, QWidget* parent = nullptr);
	~KnobControl() override = default;
};


class ComboControl : public Control<int>
{
	QWidget* m_widget;
	ComboBox* m_combo;
	QLabel* m_label;

public:
	void setText(const QString& text) override;
	QWidget* topWidget() override { return m_widget; }

	IntModelView* modelView() override;

	ComboControl(IComboBoxModelWrapper* _model, QWidget* parent = nullptr);
	~ComboControl() override = default;
};


class LcdControl : public Control<int>
{
	class LcdSpinBox* m_lcd;

public:
	void setText(const QString& text) override;
	QWidget* topWidget() override;

	IntModelView* modelView() override;

	LcdControl(int numDigits, IIntAutomatableModel* _model, QWidget* parent = nullptr);
	~LcdControl() override = default;
};


class CheckControl : public Control<bool>
{
	QWidget* m_widget;
	LedCheckBox* m_checkBox;
	QLabel* m_label;

public:
	void setText(const QString& text) override;
	QWidget* topWidget() override;

	BoolModelView* modelView() override;

	CheckControl(IBoolAutomatableModel* _model, QWidget* parent = nullptr);
	~CheckControl() override = default;
};


} // namespace gui

} // namespace lmms

#endif // LMMS_GUI_CONTROLS_H
