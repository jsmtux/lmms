/*
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

#ifndef LMMS_I_CONTROLLER_H
#define LMMS_I_CONTROLLER_H

#include <QObject>
#include <QString>

#include "ValueBuffer.h"
#include "JournallingObject.h"
#include "IModels.h"
#include "ISampleBuffer.h"

namespace lmms
{

class IControllerConnection;
class ISong;
class Model;

class IController : public QObject, public JournallingObject
{
	Q_OBJECT
public:
	enum ControllerTypes
	{
		DummyController,
		LfoController,
		MidiController,
		PeakController,
		/*
		XYController,
		EquationController
		*/
		NumControllerTypes
	} ;
    virtual ~IController() = default;
    virtual bool isSampleExact() const = 0;
    virtual ControllerTypes type() const = 0;
    virtual const QString & name() const = 0;
	virtual void setName( const QString & _new_name ) = 0;
	virtual float currentValue( int _offset ) = 0;
	virtual ValueBuffer * valueBuffer() = 0;
	virtual void addConnection( IControllerConnection * ) = 0;
	virtual void removeConnection( IControllerConnection * ) = 0;
	virtual int connectionCount() const = 0;
	virtual bool hasModel( const Model * m ) const = 0;
	virtual bool frequentUpdates() const = 0;
	virtual int getRunningFrames() = 0;
	virtual Model* model() = 0;
signals:
	// The value changed while the audio engine isn't running (i.e: MIDI CC)
	void valueChanged();
};

void TriggerControllerFrameCounter();

using ControllerVector = QVector<IController*>;

class ILfoController
{
public:
    virtual ~ILfoController() = default;
	virtual IFloatAutomatableModel& baseModel() = 0;
	virtual ITempoSyncKnobModelWrapper& speedModel() = 0;
	virtual IFloatAutomatableModel& amountModel() = 0;
	virtual IFloatAutomatableModel& phaseModel() = 0;
	virtual ISampleBuffer* userDefSampleBuffer() = 0;
	virtual IIntAutomatableModel& waveModel() = 0;
	virtual IIntAutomatableModel& multiplierModel() = 0;
};

IController* createLfoController(ISong* song);

class IPeakController
{
public:
    virtual ~IPeakController() = default;
};



} // namespace lmms

#endif // LMMS_I_CONTROLLER_H
