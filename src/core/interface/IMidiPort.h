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

#ifndef LMMS_I_MIDI_PORT_H
#define LMMS_I_MIDI_PORT_H

#include <QObject>

#include "IModels.h"

namespace lmms
{


namespace gui
{

class MidiPortMenu;

}


class MidiPortModel : public QObject
{
	Q_OBJECT
public:
	virtual ~MidiPortModel() = default;

signals:
	void readablePortsChanged();
	void writablePortsChanged();
	void modeChanged();
};

// class for abstraction of MIDI-port
class IMidiPort
{
public:
	using Map = QMap<QString, bool>;
	enum Modes
	{
		Disabled,	// don't route any MIDI-events (default)
		Input,		// from MIDI-client to MIDI-event-processor
		Output,		// from MIDI-event-processor to MIDI-client
		Duplex		// both directions
	} ;

	using Mode = Modes;

	virtual int baseVelocity() = 0;
	virtual void setName( const QString& name ) = 0;
	virtual IIntAutomatableModel* inputControllerModel() = 0;
	virtual IBoolAutomatableModel* readableModel() = 0;
	virtual IBoolAutomatableModel* writableModel() = 0;
	virtual IIntAutomatableModel* inputChannelModel() = 0;
	virtual IIntAutomatableModel* outputChannelModel() = 0;
	virtual IIntAutomatableModel* fixedInputVelocityModel() = 0;
	virtual IIntAutomatableModel* fixedOutputVelocityModel() = 0;
	virtual IIntAutomatableModel* fixedOutputNoteModel() = 0;
	virtual IIntAutomatableModel* outputProgramModel() = 0;
	virtual IIntAutomatableModel* baseVelocityModel() = 0;
	// This should be moved somewhere in the GUI
	virtual gui::MidiPortMenu* readablePortsMenu() = 0;
	virtual gui::MidiPortMenu* writablePortsMenu() = 0;
	virtual void setReadablePortsMenu(gui::MidiPortMenu* menu) = 0;
	virtual void setWritablePortsMenu(gui::MidiPortMenu* menu) = 0;
	virtual void subscribeReadablePort( const QString& port, bool subscribe = true ) = 0;
	virtual void subscribeWritablePort( const QString& port, bool subscribe = true ) = 0;
	virtual const Map& readablePorts() const = 0;
	virtual const Map& writablePorts() const = 0;
	virtual Model* model() = 0;
	virtual MidiPortModel* midiPortModel() = 0;
	virtual bool isReadable() = 0;
	virtual void setReadable(bool value) = 0;
	virtual bool isWritable() = 0;
	virtual void setWritable(bool value) = 0;
};

} // namespace lmms

#endif // L_IMMS_MIDI_PORT_H
