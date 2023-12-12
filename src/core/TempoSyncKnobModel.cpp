/*
 * TempoSyncKnobModel.cpp - adds bpm to ms conversion for knob class
 *
 * Copyright (c) 2005-2007 Danny McRae <khjklujn/at/yahoo.com>
 * Copyright (c) 2005-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include <QDomElement>

#include "TempoSyncKnobModel.h"
#include "Engine.h"
#include "Song.h"


namespace lmms
{


ITempoSyncKnobModelWrapper* MFact::create(
	float val, float min, float max, float step, float scale,
	QObject * parent,
	const QString& displayName)
{
	return new TempoSyncKnobModelWrapper(
		val, min, max, step, scale, parent, displayName);
}

TempoSyncKnobModelWrapper::TempoSyncKnobModelWrapper( const float _val, const float _min,
				const float _max, const float _step,
				const float _scale, QObject * _parent,
				const QString & _display_name ) :
	m_model( _val, _min, _max, _step, _parent, _display_name ),
	m_tempoSyncMode( SyncNone ),
	m_tempoLastSyncMode( SyncNone ),
	m_scale( _scale ),
	m_custom( _parent )
{
	connect( Engine::getSong(), SIGNAL(tempoChanged(lmms::bpm_t)),
			this, SLOT(calculateTempoSyncTime(lmms::bpm_t)),
			Qt::DirectConnection );
}



void TempoSyncKnobModelWrapper::setTempoSync( int _note_type )
{
	setSyncMode( ( TempoSyncMode ) _note_type );
	Engine::getSong()->setModified();
}




void TempoSyncKnobModelWrapper::calculateTempoSyncTime( bpm_t _bpm )
{
	float conversionFactor = 1.0;
	
	if( m_tempoSyncMode )
	{
		switch( m_tempoSyncMode )
		{
			case SyncCustom:
				conversionFactor = 
			static_cast<float>( m_custom.getDenominator() ) /
			static_cast<float>( m_custom.getNumerator() );
				break;
			case SyncDoubleWholeNote:
				conversionFactor = 0.125;
				break;
			case SyncWholeNote:
				conversionFactor = 0.25;
				break;
			case SyncHalfNote:
				conversionFactor = 0.5;
				break;
			case SyncQuarterNote:
				conversionFactor = 1.0;
				break;
			case SyncEighthNote:
				conversionFactor = 2.0;
				break;
			case SyncSixteenthNote:
				conversionFactor = 4.0;
				break;
			case SyncThirtysecondNote:
				conversionFactor = 8.0;
				break;
			default: ;
		}
		bool journalling = m_model.testAndSetJournalling( false );
		float oneUnit = 60000.0 / ( _bpm * conversionFactor * m_scale );
		m_model.setValue( oneUnit * m_model.maxValue() );
		m_model.setJournalling( journalling );
	}

	if( m_tempoSyncMode != m_tempoLastSyncMode )
	{
		emit m_signals.syncModeChanged( );
		m_tempoLastSyncMode = m_tempoSyncMode;
	}
}




void TempoSyncKnobModelWrapper::saveSettings( QDomDocument & _doc, QDomElement & _this,
							const QString & _name )
{
	_this.setAttribute( _name + "_syncmode", (int) syncMode() );
	m_custom.saveSettings( _doc, _this, _name );
	m_model.saveSettings( _doc, _this, _name );
}




void TempoSyncKnobModelWrapper::loadSettings( const QDomElement & _this,
							const QString & _name )
{
	m_model.loadSettings( _this, _name );
	m_custom.loadSettings( _this, _name );
	setSyncMode( ( TempoSyncMode ) _this.attribute( _name + "_syncmode" ).toInt() );
}




void TempoSyncKnobModelWrapper::setSyncMode( TempoSyncMode _new_mode )
{
	if( m_tempoSyncMode != _new_mode )
	{
		m_tempoSyncMode = _new_mode;
		if( _new_mode == SyncCustom )
		{
			connect( &m_custom, &Model::dataChanged,
					this, &TempoSyncKnobModelWrapper::updateCustom,
					Qt::DirectConnection );
		}
	}
	calculateTempoSyncTime( Engine::getSong()->getTempo() );
}




void TempoSyncKnobModelWrapper::setScale( float _new_scale )
{
	m_scale = _new_scale;
	calculateTempoSyncTime( Engine::getSong()->getTempo() );
	emit m_signals.scaleChanged( _new_scale );
}




void TempoSyncKnobModelWrapper::updateCustom()
{
	setSyncMode( SyncCustom );
}


} // namespace lmms
