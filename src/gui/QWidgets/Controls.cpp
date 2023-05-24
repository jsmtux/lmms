/*
 * Controls.cpp - labeled control widgets
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

#include "Controls.h"

#include "widgets/ComboBox.h"
#include "widgets/Knob.h"
#include "widgets/LcdSpinBox.h"
#include "widgets/LedCheckBox.h"

#include <QLabel>
#include <QString>
#include <QVBoxLayout>


namespace lmms::gui
{


void KnobControl::setText(const QString &text) { m_knob->setLabel(text); }

QWidget *KnobControl::topWidget() { return m_knob; }

FloatModelView* KnobControl::modelView() { return m_knob; }

KnobControl::KnobControl(IFloatAutomatableModel* _model, QWidget *parent) :
	m_knob(new Knob(_model, parent)) {}


void ComboControl::setText(const QString &text) { m_label->setText(text); }

IntModelView* ComboControl::modelView() { return m_combo; }

ComboControl::ComboControl(IComboBoxModelWrapper* _model, QWidget *parent) :
	m_widget(new QWidget(parent)),
	m_combo(new ComboBox(_model, parent)),
	m_label(new QLabel(m_widget))
{
	m_combo->setFixedSize(64, ComboBox::DEFAULT_HEIGHT);
	auto vbox = new QVBoxLayout(m_widget);
	vbox->addWidget(m_combo);
	vbox->addWidget(m_label);
	m_combo->repaint();
}



void CheckControl::setText(const QString &text) { m_label->setText(text); }

QWidget *CheckControl::topWidget() { return m_widget; }

BoolModelView* CheckControl::modelView() { return m_checkBox; }

CheckControl::CheckControl(IBoolAutomatableModel* _model, QWidget *parent) :
	m_widget(new QWidget(parent)),
	m_checkBox(new LedCheckBox(_model, parent, QString(), LedCheckBox::Green)),
	m_label(new QLabel(m_widget))
{
	auto vbox = new QVBoxLayout(m_widget);
	vbox->addWidget(m_checkBox);
	vbox->addWidget(m_label);
}




void LcdControl::setText(const QString &text) { m_lcd->setLabel(text); }

QWidget *LcdControl::topWidget() { return m_lcd; }

IntModelView* LcdControl::modelView() { return m_lcd; }

LcdControl::LcdControl(int numDigits, IIntAutomatableModel* _model, QWidget *parent) :
	m_lcd(new LcdSpinBox(numDigits, _model, parent))
{
}


} // namespace lmms::gui
