/*
 * MidiPort.h - abstraction of MIDI ports which are part of LMMS' MIDI
 *              sequencing system
 *
 * Copyright (c) 2005-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_MIDI_PORT_H
#define LMMS_MIDI_PORT_H

#include <QString>
#include <QList>
#include <QMap>

#include "IMidiPort.h"
#include "MidiConstants.h"
#include "TimePos.h"
#include "AutomatableModel.h"

namespace lmms
{

class MidiClient;
class MidiEvent;
class MidiEventProcessor;


// class for abstraction of MIDI-port
class MidiPort : public Model, public SerializingObject, public IMidiPort
{
	Q_OBJECT
	mapPropertyFromModel(int,inputChannel,setInputChannel,m_inputChannelModel);
	mapPropertyFromModel(int,outputChannel,setOutputChannel,m_outputChannelModel);
	mapPropertyFromModel(int,inputController,setInputController,m_inputControllerModel);
	mapPropertyFromModel(int,outputController,setOutputController,m_outputControllerModel);
	mapPropertyFromModel(int,fixedInputVelocity,setFixedInputVelocity,m_fixedInputVelocityModel);
	mapPropertyFromModel(int,fixedOutputVelocity,setFixedOutputVelocity,m_fixedOutputVelocityModel);
	mapPropertyFromModel(int,fixedOutputNote,setFixedOutputNote,m_fixedOutputNoteModel);
	mapPropertyFromModel(int,outputProgram,setOutputProgram,m_outputProgramModel);
public:
	MidiPort( const QString& name,
			MidiClient* client,
			MidiEventProcessor* eventProcessor,
			QObject* parent = nullptr,
			Mode mode = Disabled );
	~MidiPort() override;

	void setName( const QString& name ) override;

	Mode mode() const
	{
		return m_mode;
	}

	void setMode( Mode mode );

	bool isInputEnabled() const
	{
		return mode() == Input || mode() == Duplex;
	}

	bool isOutputEnabled() const
	{
		return mode() == Output || mode() == Duplex;
	}

	int realOutputChannel() const
	{
		// There's a possibility of outputChannel being 0 ("--"), which is used to keep all
		// midi channels when forwarding. In that case, realOutputChannel will return the
		// default channel 1 (whose value is 0).
		return outputChannel() ? outputChannel() - 1 : 0;
	}

	void processInEvent( const MidiEvent& event, const TimePos& time = TimePos() );
	void processOutEvent( const MidiEvent& event, const TimePos& time = TimePos() );


	void saveSettings( QDomDocument& doc, QDomElement& thisElement ) override;
	void loadSettings( const QDomElement& thisElement ) override;

	QString nodeName() const override
	{
		return "midiport";
	}

	void subscribeReadablePort( const QString& port, bool subscribe = true ) override;
	void subscribeWritablePort( const QString& port, bool subscribe = true ) override;

	const Map& readablePorts() const override
	{
		return m_readablePorts;
	}

	const Map& writablePorts() const override
	{
		return m_writablePorts;
	}

	void invalidateCilent();


	int baseVelocity() override {
		return m_baseVelocityModel.value();
	}
	void setBaseVelocity(int _velocity) {
		m_baseVelocityModel.setValue(_velocity);
	}

	IIntAutomatableModel* inputControllerModel() override
	{
		return &m_inputControllerModel;
	}
	IBoolAutomatableModel* readableModel() override
	{
		return &m_readableModel;
	}
	IBoolAutomatableModel* writableModel() override
	{
		return &m_writableModel;
	}
	IIntAutomatableModel* inputChannelModel() override
	{
		return &m_inputChannelModel;
	}
	IIntAutomatableModel* outputChannelModel() override
	{
		return &m_outputChannelModel;
	}
	IIntAutomatableModel* fixedInputVelocityModel() override
	{
		return &m_fixedInputVelocityModel;
	}
	IIntAutomatableModel* fixedOutputVelocityModel() override
	{
		return &m_fixedOutputVelocityModel;
	}
	IIntAutomatableModel* fixedOutputNoteModel() override
	{
		return &m_fixedOutputNoteModel;
	}
	IIntAutomatableModel* outputProgramModel() override
	{
		return &m_outputProgramModel;
	}
	IIntAutomatableModel* baseVelocityModel() override
	{
		return &m_baseVelocityModel;
	}

	gui::MidiPortMenu* readablePortsMenu() override
	{
		return m_readablePortsMenu;
	}
	gui::MidiPortMenu* writablePortsMenu() override
	{
		return m_writablePortsMenu;
	}

	void setReadablePortsMenu(gui::MidiPortMenu* menu) override
	{
		m_readablePortsMenu = menu;
	}
	void setWritablePortsMenu(gui::MidiPortMenu* menu) override
	{
		m_writablePortsMenu = menu;
	}

	Model* model() override {
		return this;
	}
	MidiPortModel* midiPortModel() override {
		return &m_midiPortModel;
	}

	bool isReadable() override
	{
		return m_readableModel.value();
	}
	
	void setReadable(bool value) override
	{
		m_readableModel.setValue(value);
	}

	bool isWritable() override
	{
		return m_writableModel.value();
	}
	
	void setWritable(bool value) override
	{
		m_writableModel.setValue(value);
	}

public slots:
	void updateMidiPortMode();


private slots:
	void updateReadablePorts();
	void updateWritablePorts();
	void updateOutputProgram();


private:
	MidiClient* m_midiClient;
	MidiEventProcessor* m_midiEventProcessor;

	Mode m_mode;

	IntModel m_inputChannelModel;
	IntModel m_outputChannelModel;
	IntModel m_inputControllerModel;
	IntModel m_outputControllerModel;
	IntModel m_fixedInputVelocityModel;
	IntModel m_fixedOutputVelocityModel;
	IntModel m_fixedOutputNoteModel;
	IntModel m_outputProgramModel;
	IntModel m_baseVelocityModel;
	BoolModel m_readableModel;
	BoolModel m_writableModel;

	Map m_readablePorts;
	Map m_writablePorts;

	gui::MidiPortMenu* m_readablePortsMenu;
	gui::MidiPortMenu* m_writablePortsMenu;
	MidiPortModel m_midiPortModel;
} ;

using MidiPortList = QList<MidiPort*>;

} // namespace lmms

#endif // LMMS_MIDI_PORT_H
