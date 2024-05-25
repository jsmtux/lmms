/*
 * Kicker.h - drum synthesizer
 *
 * Copyright (c) 2006-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2014 grejppi <grejppi/at/gmail.com>
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


#ifndef KICKER_GUI_H
#define KICKER_GUI_H

#include "GuiApplication.h"
#include "AudioFileProcessor.h"


namespace lmms
{
namespace gui
{

class AudioFileProcessorModel : public InstrumentModel
{
    Q_OBJECT
public:
    AudioFileProcessorModel(QObject* parent, IInstrument* _instrument)
        : InstrumentModel(parent, _instrument)
    {
        qWarning() << "Creating processor\n";
    }
};

class AudioFileProcessorQmlInstrument : public AudioFileProcessor, public ModelFactory {
    Q_OBJECT
public:
    AudioFileProcessorQmlInstrument(IInstrumentTrack * _instrument_track)
		: AudioFileProcessor(_instrument_track)
	{
        qWarning() << "Creating audiofileprocessorquimlinstrument\n\n";
    }

    ~AudioFileProcessorQmlInstrument() {
        qWarning() << "DELETING audiofileprocessorquimlinstrument\n\n";
    }

	IGUISpecificPlugin* guiSpecificPlugin() override {
        return this;
    }

	InstrumentModel* getModel(QObject* parent) override {
        qWarning() << "Returning model\n";
        return new AudioFileProcessorModel(parent, this);
    }
};

} // namespace gui
} // namespace lmms

#endif
