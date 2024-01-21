/*
 * AutomatableModelView.h - provides AutomatableModelView base class and
 * provides BoolModelView, FloatModelView, IntModelView subclasses.
 *
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

#ifndef LMMS_GUI_AUTOMATABLE_MODEL_VIEW_H
#define LMMS_GUI_AUTOMATABLE_MODEL_VIEW_H

#include "IModels.h"

class QMenu;
class QMouseEvent;

namespace lmms::gui
{

class LMMS_EXPORT AutomatableModelViewBase
{
public:
	AutomatableModelViewBase( IAutomatableModelBase* _model, QWidget* _widget );
	~AutomatableModelViewBase() = default;

	inline void setDescription( const QString& desc )
	{
		m_description = desc;
	}

	inline void setUnit( const QString& unit )
	{
		m_unit = unit;
	}

	IAutomatableModelBase* baseModel()
	{
		return m_model;
	}

	void addDefaultActions( QMenu* menu );

	void setConversionFactor( float factor );
	float getConversionFactor();

protected:
	template<class ModelType>
	IAutomatableModel<ModelType>* castModel() {
		return static_cast<IAutomatableModel<ModelType>*>(m_model);
	}

	template<class ModelType>
	const IAutomatableModel<ModelType>* castModel() const {
		return static_cast<const IAutomatableModel<ModelType>*>(m_model);
	}

	virtual void mousePressEvent( QMouseEvent* event );

	QWidget* m_widget;
	QString m_description;
	QString m_unit;
	float m_conversionFactor; // Factor to be applied when the m_model->value is displayed
	IAutomatableModelBase* m_model;
} ;


template <typename ModelType>
class LMMS_EXPORT AutomatableModelView : public AutomatableModelViewBase
{
public:
	AutomatableModelView( IAutomatableModel<ModelType>* model, QWidget* _this) :
		AutomatableModelViewBase( model, _this )
	{}

	IAutomatableModel<ModelType>* model()
	{
		return castModel<ModelType>();
	}
	const IAutomatableModel<ModelType>* model() const
	{
		return castModel<ModelType>();
	}
};

using FloatModelView = AutomatableModelView<float>;
using IntModelView = AutomatableModelView<int>;
using BoolModelView = AutomatableModelView<bool>;


class AutomatableModelViewSlots : public QObject
{
	Q_OBJECT
public:
	AutomatableModelViewSlots( AutomatableModelViewBase* amv, QObject* parent );

public slots:
	void execConnectionDialog();
	void removeConnection();
	void editSongGlobalAutomation();
	void unlinkAllModels();
	void removeSongGlobalAutomation();

private slots:
	/// Copy the model's value to the clipboard.
	void copyToClipboard();
	/// Paste the model's value from the clipboard.
	void pasteFromClipboard();

protected:
	AutomatableModelViewBase* m_amv;
} ;

} // namespace lmms::gui

#endif // LMMS_GUI_AUTOMATABLE_MODEL_VIEW_H
