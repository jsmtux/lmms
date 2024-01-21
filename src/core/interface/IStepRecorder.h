/*
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

#ifndef LMMS_I_STEP_RECORDER_H
#define LMMS_I_STEP_RECORDER_H

#include <QVector>

class QKeyEvent;

namespace lmms
{

class IMidiClip;
class Note;
class TimePos;

namespace gui
{
class IPianoRoll;
class IStepRecorderWidget;
} // namespace gui

class IStepRecorder
{
public:
	virtual ~IStepRecorder() = default;
	virtual bool isRecording() const = 0;
	virtual void initialize() = 0;
	virtual void start(const TimePos& currentPosition,const TimePos& stepLength) = 0;
	virtual void stop() = 0;
	virtual void setCurrentMidiClip(IMidiClip* newMidiClip) = 0;
	virtual bool keyPressEvent(QKeyEvent* ke) = 0;
	virtual QVector<Note*> getCurStepNotes() = 0;
	virtual QColor curStepNoteColor() const = 0;
	virtual void notePressed(const Note & n) = 0;
	virtual void noteReleased(const Note & n) = 0;
	virtual void setStepsLength(const TimePos& newLength) = 0;
};

IStepRecorder* createStepRecorder(gui::IPianoRoll& pianoRoll, gui::IStepRecorderWidget& stepRecorderWidget);

}

#endif // LMMS_I_STEP_RECORDER_H