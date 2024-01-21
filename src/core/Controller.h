/*
 * Controller.h - declaration of class controller, which provides a
 *                standard for all controllers and controller plugins
 *
 * Copyright (c) 2008-2009 Paul Giblock <pgllama/at/gmail.com>
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

#ifndef LMMS_CONTROLLER_H
#define LMMS_CONTROLLER_H

#include "IController.h"

#include "lmms_export.h"
#include "Engine.h"
#include "Model.h"
#include "ValueBuffer.h"

namespace lmms
{

class Controller;
class IControllerConnection;

namespace gui
{

class ControllerDialog;

} // namespace gui

class LMMS_EXPORT Controller : public IController
{
	Q_OBJECT
public:
	Controller( ControllerTypes _type, QObject * _parent,
						const QString & _display_name );

	~Controller() override;

	virtual float currentValue( int _offset ) override;
	// The per-controller get-value-in-buffers function
	virtual ValueBuffer * valueBuffer() override;

	inline bool isSampleExact() const override
	{
		return m_sampleExact;
	}

	void setSampleExact( bool _exact )
	{
		m_sampleExact = _exact;
	}

	inline ControllerTypes type() const override
	{
		return( m_type );
	}

	// return whether this controller updates models frequently - used for
	// determining when to update GUI
	inline bool frequentUpdates() const override
	{
		switch( m_type )
		{
			case LfoController: return( true );
			case PeakController: return( true );
			default:
				break;
		}
		return( false );
	}

	virtual const QString & name() const override
	{
		return( m_name );
	}


	void saveSettings( QDomDocument & _doc, QDomElement & _this ) override;
	void loadSettings( const QDomElement & _this ) override;
	QString nodeName() const override;

	static Controller * create( ControllerTypes _tt, QObject * _parent );
	static Controller * create( const QDomElement & _this,
							QObject * _parent );

	inline static float fittedValue( float _val )
	{
		return qBound<float>( 0.0f, _val, 1.0f );
	}

	static long runningPeriods()
	{
		return s_periods;
	}
	static unsigned int runningFrames();
	int getRunningFrames() override {
		return runningFrames();
	}
	static float runningTime();

	static void triggerFrameCounter();
	static void resetFrameCounter();

	//Accepts a ControllerConnection * as it may be used in the future.
	void addConnection( IControllerConnection * ) override;
	void removeConnection( IControllerConnection * ) override;
	int connectionCount() const override;

	bool hasModel( const Model * m ) const override;

	Model* model() override {
		return &m_model;
	}

public slots:
	virtual void setName( const QString & _new_name ) override
	{
		m_name = _new_name;
	}

private:
	Model m_model;

protected:
	// The internal per-controller get-value function
	virtual float value( int _offset );

	virtual void updateValueBuffer();

	// buffer for storing sample-exact values in case there
	// are more than one model wanting it, so we don't have to create it
	// again every time
	ValueBuffer m_valueBuffer;
	// when we last updated the valuebuffer - so we know if we have to update it
	long m_bufferLastUpdated;

	float m_currentValue;
	bool  m_sampleExact;
	int m_connectionCount;

	QString m_name;
	ControllerTypes m_type;

	static QVector<Controller*> s_controllers;

	static long s_periods;

} ;


} // namespace lmms

#endif // LMMS_CONTROLLER_H
