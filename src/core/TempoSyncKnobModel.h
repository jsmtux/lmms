/*
 * TempoSyncKnobModelWrapper.h - adds bpm to ms conversion for knob class
 *
 * Copyright (c) 2005-2008 Danny McRae <khjklujn/at/yahoo.com>
 * Copyright (c) 2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_TEMPO_SYNC_KNOB_MODEL_H
#define LMMS_TEMPO_SYNC_KNOB_MODEL_H

#include "IModels.h"
#include "MeterModel.h"

namespace lmms
{

namespace gui
{

class TempoSyncKnob;

} // namespace gui


class TempoSyncKnobModelWrapper : public QObject, public ITempoSyncKnobModelWrapper
{
	Q_OBJECT
public:
	TempoSyncKnobModelWrapper( const float _val, const float _min,
				const float _max, const float _step,
				const float _scale, QObject * _parent,
				const QString & _display_name = QString() );
	~TempoSyncKnobModelWrapper() override = default;

	void saveSettings( QDomDocument & _doc, QDomElement & _this, const QString& name ) override;
	void loadSettings( const QDomElement & _this, const QString& name ) override;

	TempoSyncMode syncMode() const
	{
		return m_tempoSyncMode;
	}

	void setSyncMode( TempoSyncMode _new_mode );

	float scale() const override
	{
		return m_scale;
	}

	void setScale( float _new_scale );

	void setRange( const float min, const float max, const float step) override
	{
		m_model.setRange(min, max, step);
	}

	float initValue() override
	{
		return m_model.initValue();
	}

	void setInitValue( const float value ) override
	{
		m_model.setInitValue(value);
	}

	void setScaleLogarithmic( bool setToTrue ) override
	{
		m_model.setScaleLogarithmic(setToTrue);
	}

    ValueBuffer * valueBuffer() override
	{
		return m_model.valueBuffer();
	}

    void linkModel(ITempoSyncKnobModelWrapper* another) override
	{
		FloatModel::linkModels(&m_model,
			&static_cast<TempoSyncKnobModelWrapper*>(another)->m_model);
	}

    void unlinkModel(ITempoSyncKnobModelWrapper* another) override
	{
		FloatModel::unlinkModels(&m_model,
			&static_cast<TempoSyncKnobModelWrapper*>(another)->m_model);
	}

    const IFloatAutomatableModel* wrappedModel() const override
	{
		return &m_model;
	}

    IFloatAutomatableModel* wrappedModel() override
	{
		return &m_model;
	}
	
	TempoSyncMode tempoSyncMode() override
	{
		return m_tempoSyncMode;
	}

	IMeterModel* custom() override
	{
		return &m_custom;
	}

	TempoSyncKnobSignals* modelSignals() override
	{
		return &m_signals;
	}


public slots:
	inline void disableSync()
	{
		setTempoSync( SyncNone );
	}
	void setTempoSync( int _note_type ) override;


protected slots:
	void calculateTempoSyncTime( lmms::bpm_t _bpm );
	void updateCustom();


private:
	FloatModel m_model;
	TempoSyncKnobSignals m_signals;
	TempoSyncMode m_tempoSyncMode;
	TempoSyncMode m_tempoLastSyncMode;
	float m_scale;

	MeterModel m_custom;

	friend class gui::TempoSyncKnob;

} ;

} // namespace lmms

#endif // LMMS_TEMPO_SYNC_KNOB_MODEL_H
