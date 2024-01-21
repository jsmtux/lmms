/*
 * Plugin.h - class plugin, the base-class and generic interface for all plugins
 *
 * Copyright (c) 2005-2023 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_I_CLIP_H
#define LMMS_I_CLIP_H

#include <QObject>
#include "TimePos.h"
#include "IModels.h"
#include "AutomationNode.h"
#include "JournallingObject.h"
#include "Note.h"

namespace lmms
{

class ITrack;

enum class ClipType
{
	Automation = 0,
	Midi,
	Pattern,
	Sample
};

QString ClipTypeToString(const ClipType _type);
std::size_t ClipTypeToId(const ClipType _type);

class IClip;

class ClipTypeSpecificInterface{
public:
    virtual ~ClipTypeSpecificInterface() = default;
    virtual IClip* baseClip() = 0;
    virtual const IClip* baseClip() const = 0;
};

class IClip : public QObject, public JournallingObject
{
Q_OBJECT
public:
    virtual ~IClip() = default;
    virtual const TimePos & length() const = 0;
    virtual void movePosition( const TimePos & pos ) = 0;
    virtual const TimePos & startPosition() const = 0;
    virtual TimePos endPosition() const = 0;
    virtual bool isMuted() const = 0;
    virtual void changeLength( const TimePos & length ) = 0;
    virtual TimePos startTimeOffset() const = 0;
	virtual void setStartTimeOffset( const TimePos &startTimeOffset ) = 0;
    virtual Model* model() = 0;
    virtual QString name() const = 0;
    virtual void setName( const QString & name ) = 0;
    virtual ITrack* getITrack() const = 0;
	virtual ClipType getType() = 0;
    virtual bool usesCustomClipColor() = 0;
    virtual QColor color() const = 0;
	virtual void* getTrackId() const = 0;
	virtual void addJournalCheckPointToTrack() = 0;
    virtual void useCustomClipColor( bool b ) = 0;
    virtual void setColor( const QColor & c ) = 0;
    virtual const bool getAutoResize() const = 0;
	virtual bool hasColor() = 0;
    virtual void toggleMute() = 0;
	virtual bool isClipOrTrackMuted() = 0;
    virtual void selectViewOnCreate( bool select ) = 0;
    virtual bool getSelectViewOnCreate() = 0;
	// Return track color unless clip has a custom one
	virtual QColor getEffectiveColor() = 0;

	virtual ClipTypeSpecificInterface* getClipTypeSpecificInterface() = 0;

signals:
	void destroyedClip();
};

class IAutomationClip : public AutomationOwner, public ClipTypeSpecificInterface
{
public:
	using objectVector = QVector<IAutomatableModelBase*>;
	enum ProgressionTypes
	{
		DiscreteProgression,
		LinearProgression,
		CubicHermiteProgression
	} ;
    virtual ~IAutomationClip() {}
    static bool isAutomated( const IAutomatableModelBase * _m );
    virtual const objectVector & getObjects() const = 0;
    virtual objectVector & getObjects() = 0;
    virtual const IAutomatableModelBase * firstObject() const = 0;
    virtual bool isRecording() const = 0;
    virtual void setRecording( const bool b ) = 0;
    virtual void flipX( int length = -1 ) = 0;
    virtual void flipY( int min, int max ) = 0;
    virtual void flipY() = 0;
    virtual TimePos timeMapLength() const = 0;
    virtual float *valuesAfter( const TimePos & _time ) const = 0;
    virtual ProgressionTypes progressionType() const = 0;
    virtual void setProgressionType( ProgressionTypes _new_progression_type ) = 0;
    virtual bool addObject( IAutomatableModelBase * _obj, bool _search_dup = true ) = 0;
    virtual void clear() = 0;
    virtual float getTension() const = 0;
    virtual void removeNode(const TimePos & time) = 0;
    virtual float getMax() const = 0;
    virtual float getMin() const = 0;
    virtual TimePos putValue(
		const TimePos & time,
		const float value,
		const bool quantPos = true,
		const bool ignoreSurroundingPoints = true
	) = 0;
    virtual TimePos setDragValue( const TimePos & time,
				const float value,
				const bool quantPos = true,
				const bool controlKey = false ) = 0;
    
	virtual void removeNodes(const int tick0, const int tick1) = 0;
    virtual void resetNodes(const int tick0, const int tick1) = 0;

    virtual void applyDragValue() = 0;
    virtual void setTension( QString _new_tension ) = 0;
    static void setQuantization(int q);

    static void resolveAllIDs();
    static int quantization();
};

IAutomationClip* SetGlobalAutomationClip(IAutomatableModelBase* model);

class SampleClipModel : public QObject
{
    Q_OBJECT
signals:
	void sampleChanged();
	void wasReversed();
};

class ISampleBuffer;

class ISampleClip : public ClipTypeSpecificInterface {
public:
    virtual ~ISampleClip() = default;
    virtual SampleClipModel& sampleClipModel() = 0;
    virtual ISampleBuffer* iSampleBuffer() = 0;
    virtual void setSampleFile( const QString & _sf ) = 0;
    virtual void updateLength() = 0;
	virtual void toggleRecord() = 0;
    virtual void updateTrackClips() = 0;
	virtual void playbackPositionChanged() = 0;
    virtual TimePos sampleLength() const = 0;
};

class ISampleTrack;

ISampleClip* createSampleClip(ISampleClip& clip);

class IPatternClip : public ClipTypeSpecificInterface
{
public:
    virtual ~IPatternClip() = default;
    virtual int patternIndex() = 0;
};

class IInstrumentTrack;
class IMidiClip;

class MidiClipModel : public QObject
{
    Q_OBJECT
signals:
    void destroyedMidiClip(IMidiClip*);
};

class IMidiClip : public ClipTypeSpecificInterface
{
public:
	enum MidiClipTypes
	{
		BeatClip,
		MelodyClip
	} ;
    virtual ~IMidiClip() = default;
    virtual Note * addNote( const Note & _new_note, const bool _quant_pos = true ) = 0;
    virtual void updateLength() = 0;
    virtual void rearrangeAllNotes() = 0;
    virtual const NoteVector & notes() const = 0;
    virtual MidiClipTypes type() const = 0;
    virtual IInstrumentTrack* getInstrumentTrack() = 0;
    virtual bool empty() = 0;
    virtual void clear() = 0;
    virtual void addSteps() = 0;
    virtual void cloneSteps() = 0;
    virtual void removeSteps() = 0;
    virtual int steps() = 0;
    virtual Note * noteAtStep( int _step ) = 0;
    virtual Note * addStepNote( int step ) = 0;
    virtual void setStep( int step, bool enabled ) = 0;
    virtual void removeNote( Note * _note_to_del ) = 0;
    virtual MidiClipModel* midiClipModel() = 0;
    virtual IMidiClip * previousMidiClip() const = 0;
    virtual IMidiClip * nextMidiClip() const = 0;
    virtual void splitNotes(NoteVector notes, TimePos pos) = 0;
    // Wrongly used from PianoRoll, should call saveState/loadState...
	virtual void saveSettings( QDomDocument& doc, QDomElement& element ) = 0;
	virtual void loadSettings( const QDomElement& element ) = 0;
};

} // namespace lmms

#endif // LMMS_I_CLIP_H