/*
 * MidiController.h - A controller to receive MIDI control-changes
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

#ifndef LMMS_MIDI_CONTROLLER_H
#define LMMS_MIDI_CONTROLLER_H

#include <QWidget>

#include "IMidiController.h"

#include "AutomatableModel.h"
#include "Controller.h"
#include "MidiEventProcessor.h"
#include "MidiPort.h"


namespace lmms
{

class MidiPort;


class MidiController : public Controller, public MidiEventProcessor, public IMidiController
{
	Q_OBJECT
public:
	MidiController( QObject * _parent );
	~MidiController() override = default;

	void processInEvent( const MidiEvent & _me,
					const TimePos & _time, f_cnt_t offset = 0 ) override;

	void processOutEvent( const MidiEvent& _me,
					const TimePos & _time, f_cnt_t offset = 0 ) override
	{
		// No output yet
	}

	IController* baseController() override {
		return this;
	}


	// Would be a nice copy ctor, but too hard to add copy ctor because
	// model has none.
	IMidiController* copyToMidiController( Model* parent ) override
	{
		auto c = new MidiController(parent);
		c->m_midiPort.setInputChannel( m_midiPort.inputChannel() );
		c->m_midiPort.setInputController( m_midiPort.inputController() );
		c->subscribeReadablePorts( m_midiPort.readablePorts() );
		c->updateName();

		return c;
	}

	void saveSettings( QDomDocument & _doc, QDomElement & _this ) override;
	void loadSettings( const QDomElement & _this ) override;
	QString nodeName() const override;

	// Used by controllerConnectionDialog to copy
	void subscribeReadablePorts( const MidiPort::Map & _map ) override;

	IController* controllerInterface() override {
		return this;
	}

	IMidiPort* midiPortInterface() override
	{
		return &m_midiPort;
	}
public slots:
	void updateName();


protected:
	// The internal per-controller get-value function
	void updateValueBuffer() override;


	MidiPort m_midiPort;


	float m_lastValue;
	float m_previousValue;
};

class AutoDetectMidiController : public MidiController, public IAutodetectMidiController
{
	Q_OBJECT
public:
	AutoDetectMidiController( QObject* parent ) :
		MidiController( parent ),
		m_detectedMidiChannel( 0 ),
		m_detectedMidiController( 0 )
	{
		updateName();
	}


	~AutoDetectMidiController() override = default;

	IAutodetectMidiController* getAutodetectIfExists() override
	{
		return this;
	}


	void processInEvent( const MidiEvent& event, const TimePos& time, f_cnt_t offset = 0 ) override;


	void useDetected() override
	{
		m_midiPort.setInputChannel( m_detectedMidiChannel );
		m_midiPort.setInputController( m_detectedMidiController );

		const MidiPort::Map& map = m_midiPort.readablePorts();
		for( MidiPort::Map::ConstIterator it = map.begin(); it != map.end(); ++it )
		{
			m_midiPort.subscribeReadablePort( it.key(),
									m_detectedMidiPort.isEmpty() || ( it.key() == m_detectedMidiPort ) );
		}
	}


public slots:
	void reset() override
	{
		m_midiPort.setInputChannel( 0 );
		m_midiPort.setInputController( 0 );
	}


private:
	int m_detectedMidiChannel;
	int m_detectedMidiController;
	QString m_detectedMidiPort;

};


} // namespace lmms

#endif // LMMS_MIDI_CONTROLLER_H
