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

#ifndef LMMS_I_TRACK_H
#define LMMS_I_TRACK_H

#include <QVector>
#include <QObject>
#include "IClip.h"
#include "lmms_basics.h"
#include "JournallingObject.h"
#include "PluginDescriptor.h"
#include "Note.h"
#include "IAudioPort.h"

namespace lmms
{

class IDataFile;

class IInstrument;
class ITrackContainer;

class TrackTypeSpecificInterface {
public:
	virtual ITrack* baseTrack() = 0;
};


/*! The minimum track height in pixels
 *
 * Tracks can be resized by shift-dragging anywhere inside the track
 * display.  This sets the minimum size in pixels for a track.
 */
const int MINIMAL_TRACK_HEIGHT = 32;
const int DEFAULT_TRACK_HEIGHT = 32;

class ITrack : public QObject, public JournallingObject
{
Q_OBJECT
public:
	enum TrackTypes
	{
		InstrumentTrack,
		PatternTrack,
		SampleTrack,
		EventTrack,
		VideoTrack,
		AutomationTrack,
		HiddenAutomationTrack,
		NumTrackTypes
	} ;
    virtual TrackTypes type() const = 0;
	using clipVector = QVector<IClip*>;

    virtual Model* model() = 0;
	virtual const QString & name() const = 0;
	virtual void setName( const QString & newName ) = 0;

	virtual bool play( const TimePos & start, const fpp_t frames,
						const f_cnt_t frameBase, int clipNum = -1 ) = 0;

    virtual const clipVector & getClips() const = 0;
	virtual void getClipsInRange( clipVector & clipV, const TimePos & start,
							const TimePos & end ) = 0;
	virtual int numOfClips() = 0;
	virtual IClip * getClip( int clipNum ) = 0;
	virtual void swapPositionOfClips( int clipNum1, int clipNum2 ) = 0;
	virtual IClip * createClip( const TimePos & pos ) = 0;
	virtual void createClipsForPattern(int pattern) = 0;
	virtual void deleteClips() = 0;

	virtual bool isMuted() const = 0;
	virtual void setMuted( const bool val ) = 0;
	virtual void insertBar( const TimePos & pos ) = 0;
	virtual void removeBar( const TimePos & pos ) = 0;
	virtual bar_t length() const = 0;
	virtual TrackTypeSpecificInterface* getTrackTypeSpecificInterface() = 0;
	virtual ITrackContainer* trackInterfaceContainer() const = 0;
	virtual ITrack * clone() = 0;
	virtual void lock() = 0;
	virtual void unlock() = 0;
	virtual void setSimpleSerializing() = 0;
	virtual int getHeight() = 0;
	virtual IBoolAutomatableModel* getMutedModel() = 0;
	virtual bool useColor() = 0;
	virtual QColor color() = 0;
	virtual void setHeight( int height ) = 0;
	virtual void setColor(const QColor& c) = 0;
	virtual void resetColor() = 0;
	virtual IBoolAutomatableModel* soloModel() = 0;
signals:
	void nameChanged();
	void colorChanged();
};

ITrack* createTrack( const QDomElement & element, ITrackContainer* tc );

using TrackList = QVector<ITrack*>;
class IPiano;

class SampleTrackSignals : public QObject
{
	Q_OBJECT
signals:
	void playingChanged();
};

class ISampleTrack : public TrackTypeSpecificInterface
{
public:
	virtual ~ISampleTrack() = default;
	virtual ISampleClip* createSampleClip(const TimePos & pos) = 0;
	virtual void createSampleClip(const TimePos & pos, const QString & sampleFile ) = 0;
	virtual IFloatAutomatableModel* volumeModel() = 0;
	virtual IFloatAutomatableModel* panningModel() = 0;
	virtual IIntAutomatableModel* mixerChannelModel() = 0;
	virtual IAudioPort* audioPortInterface() = 0;
	virtual SampleTrackSignals* sampleTrackSignals() = 0;
	virtual bool isPlaying() = 0;
};

ISampleTrack* createSampleTrack(ITrackContainer* tc);

class InstrumentTrackModel : public Model
{
	Q_OBJECT
public:
	InstrumentTrackModel(): Model(nullptr) {}
signals:
	void instrumentChanged();
	void midiNoteOn( const lmms::Note& );
	void midiNoteOff( const lmms::Note& );
	void nameChanged();
	void newNote();
	void endNote();
};

class IMidiPort;
class IMicrotuner;
class IMidiEvent;
class IInstrumentSoundShaping;
class IInstrumentFunctionNoteStacking;
class IInstrumentFunctionArpeggio;

char const *const FILENAME_FILTER = "[\\0000-\x1f\"*/:<>?\\\\|\x7f]";

class IInstrumentTrack : public TrackTypeSpecificInterface
{
public:
	virtual ~IInstrumentTrack() = default;
	virtual const IInstrument *instrument() const = 0;
	virtual IInstrument *instrument() = 0;
	virtual IInstrument * loadInstrument(const QString & _instrument_name,
				const PluginDescriptor::Key* key = nullptr,
				bool keyFromDnd = false) = 0;
	virtual void replaceInstrument(IDataFile& dataFile) = 0;

	virtual InstrumentTrackModel* instrumentTrackModel() = 0;

	virtual IFloatAutomatableModel* pitchModel() = 0;
	virtual IFloatAutomatableModel* volumeModel() = 0;
	virtual IFloatAutomatableModel* panningModel() = 0;
	virtual IIntAutomatableModel* pitchRangeModel() = 0;
	virtual IBoolAutomatableModel* midiCCEnabledModel() = 0;
	virtual IFloatAutomatableModel* midiCCModel(std::size_t index) = 0;

	virtual IIntAutomatableModel* mixerChannelModel() = 0;
	virtual IMidiClip* createMidiClip(TimePos pos) = 0;
	virtual IPiano* piano() = 0;
	virtual IIntAutomatableModel* firstKeyModel() = 0;
	virtual IIntAutomatableModel* lastKeyModel() = 0;
	virtual IMicrotuner* microtuner() = 0;
	virtual IMidiPort * midiPortInterface() = 0;
	virtual bool isKeyMapped(int key) const = 0;
	virtual void processInEvent(
		const IMidiEvent& event,
		const TimePos& time = TimePos(), f_cnt_t offset = 0 ) = 0;
	virtual void autoAssignMidiDevice( bool ) = 0;
	virtual IBoolAutomatableModel* useMasterPitchModel() = 0;
	virtual IInstrumentSoundShaping* soundShaping() = 0;
	virtual IInstrumentFunctionNoteStacking* noteStacking() = 0;
	virtual IInstrumentFunctionArpeggio* arpeggio() = 0;
	virtual QString instrumentName() const = 0;
	virtual IAudioPort * audioPortInterface() = 0;
	virtual const IIntAutomatableModel *baseNoteModel() const = 0;
	virtual IIntAutomatableModel *baseNoteModel() = 0;
	virtual bool keyRangeImport() const = 0;
	virtual int firstKey() const = 0;
	virtual int lastKey() const = 0;
	virtual int baseNote() const = 0;
	virtual void processAudioBuffer( sampleFrame * _buf, const fpp_t _frames,
							INotePlayHandle * _n )  = 0;
	// This shouldn't be used, saveState/loadState should be used instead
	virtual void saveSettings( QDomDocument& doc, QDomElement& element ) = 0;
	virtual void loadSettings( const QDomElement& element ) = 0;
};

IInstrumentTrack* createInstrumentTrack(ITrackContainer* tc);

class IAutomationTrack : public TrackTypeSpecificInterface
{
public:
	virtual IAutomationClip* createAutomationClip(TimePos pos) = 0;
};

IAutomationTrack* createAutomationTrack(ITrackContainer* tc);

class IPatternTrack : public TrackTypeSpecificInterface
{
public:
	static IPatternTrack* findPatternTrack(int pattern_num);
	virtual int patternIndex() = 0;
	static void swapPatternTracks(ITrack* track1, ITrack* track2);
	static int getInfoMap(IPatternTrack* track);
};
    
} // namespace lmms

#endif // LMMS_I_TRACK_H