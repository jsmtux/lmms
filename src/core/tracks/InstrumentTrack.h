/*
 * InstrumentTrack.h - declaration of class InstrumentTrack, a track which
 *                     holds an instrument-plugin
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_INSTRUMENT_TRACK_H
#define LMMS_INSTRUMENT_TRACK_H

#include "audio/AudioPort.h"
#include "InstrumentFunctions.h"
#include "InstrumentSoundShaping.h"
#include "Microtuner.h"
#include "MidiConstants.h"
#include "midi/MidiEventProcessor.h"
#include "midi/MidiPort.h"
#include "NotePlayHandle.h"
#include "Piano.h"
#include "TypedTrack.h"
#include "IPlugin.h"


namespace lmms
{

class MidiClip;
class Instrument;
class DataFile;

namespace gui
{

class InstrumentTrackView;
class InstrumentTrackWindow;
class InstrumentMiscView;
class MidiCCRackView;

} // namespace gui



class LMMS_EXPORT InstrumentTrack : public TypedTrack<InstrumentTrack>, public MidiEventProcessor, public IInstrumentTrack
{
	Q_OBJECT
	MM_OPERATORS
	mapPropertyFromModel(int,getVolume,setVolume,m_volumeModel);
public:
	InstrumentTrack( TrackContainer* tc );
	~InstrumentTrack() override;

	ITrack* baseTrack() override {
		return this;
	}

	// used by instrument
	void processAudioBuffer( sampleFrame * _buf, const fpp_t _frames,
							INotePlayHandle * _n ) override;

	MidiEvent applyMasterKey( const MidiEvent& event );

	void processInEvent( const MidiEvent& event, const TimePos& time = TimePos(), f_cnt_t offset = 0 ) override;
	virtual void processInEvent(
		const IMidiEvent& event,
		const TimePos& time = TimePos(), f_cnt_t offset = 0 ) override
	{
		processInEvent(static_cast<const MidiEvent&>(event), time, offset);
	}

	void processOutEvent( const MidiEvent& event, const TimePos& time = TimePos(), f_cnt_t offset = 0 ) override;
	// silence all running notes played by this track
	void silenceAllNotes( bool removeIPH = false );

	bool isSustainPedalPressed() const
	{
		return m_sustainPedalPressed;
	}

	f_cnt_t beatLen( NotePlayHandle * _n ) const;


	// for capturing note-play-events -> need that for arpeggio,
	// filter and so on
	void playNote( NotePlayHandle * _n, sampleFrame * _working_buffer );

	QString instrumentName() const override;
	const IInstrument *instrument() const override
	{
		return m_instrument;
	}

	IInstrument *instrument() override
	{
		return m_instrument;
	}

	void deleteNotePluginData( NotePlayHandle * _n );

	// name-stuff
	void setName( const QString & _new_name ) override;

	// translate given key of a note-event to absolute key (i.e.
	// add global master-pitch and base-note of this instrument track)
	int masterKey( int _midi_key ) const;

	// translate pitch to midi-pitch [0,16383]
	int midiPitch() const
	{
		return static_cast<int>( ( ( m_pitchModel.value() + m_pitchModel.range()/2 ) * MidiMaxPitchBend ) / m_pitchModel.range() );
	}

	/*! \brief Returns current range for pitch bend in semitones */
	int midiPitchRange() const
	{
		return m_pitchRangeModel.value();
	}

	// play everything in given frame-range - creates note-play-handles
	bool play( const TimePos & _start, const fpp_t _frames,
						const f_cnt_t _frame_base, int _clip_num = -1 ) override;
	// create new track-content-object = clip
	IClip* createClip(const TimePos & pos) override;


	// called by track
	void saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _parent ) override;
	void loadTrackSpecificSettings( const QDomElement & _this ) override;

	using Track::setJournalling;


	// load instrument whose name matches given one
	IInstrument * loadInstrument(const QString & _instrument_name,
				const PluginDescriptor::Key* key = nullptr,
				bool keyFromDnd = false) override;

	AudioPort * audioPort()
	{
		return &m_audioPort;
	}

	IAudioPort * audioPortInterface() override
	{
		return &m_audioPort;
	}

	MidiPort * midiPort()
	{
		return &m_midiPort;
	}

	IMidiPort* midiPortInterface() override
	{
		return midiPort();
	}

	const IIntAutomatableModel *baseNoteModel() const override
	{
		return &m_baseNoteModel;
	}

	IIntAutomatableModel *baseNoteModel() override
	{
		return &m_baseNoteModel;
	}

	IIntAutomatableModel *firstKeyModel() override
	{
		return &m_firstKeyModel;
	}

	IIntAutomatableModel *lastKeyModel() override
	{
		return &m_lastKeyModel;
	}

	IMidiClip* createMidiClip(TimePos pos) override;

	bool keyRangeImport() const override;
	bool isKeyMapped(int key) const override;
	int firstKey() const override;
	int lastKey() const override;
	int baseNote() const override;
	float baseFreq() const override;

	IPiano *piano() override
	{
		return &m_piano;
	}

	IMicrotuner *microtuner() override
	{
		return &m_microtuner;
	}

	bool isArpeggioEnabled() const
	{
		return m_arpeggio.m_arpEnabledModel.value();
	}

	// simple helper for removing midiport-XML-node when loading presets
	static void removeMidiPortNode( IDataFile& dataFile );

	IFloatAutomatableModel* pitchModel() override
	{
		return &m_pitchModel;
	}

	IFloatAutomatableModel* volumeModel() override
	{
		return &m_volumeModel;
	}

	IFloatAutomatableModel* panningModel() override
	{
		return &m_panningModel;
	}

	IIntAutomatableModel* pitchRangeModel() override
	{
		return &m_pitchRangeModel;
	}

	IIntAutomatableModel * mixerChannelModel() override
	{
		return &m_mixerChannelModel;
	}

	IBoolAutomatableModel* useMasterPitchModel() override
	{
		return &m_useMasterPitchModel;
	}

	void setPreviewMode( const bool );

	bool isPreviewMode() const
	{
		return m_previewMode;
	}
	
	void replaceInstrument(IDataFile& dataFile) override;

	void autoAssignMidiDevice( bool ) override;

	IBoolAutomatableModel* midiCCEnabledModel() override {
		return m_midiCCEnable.get();
	}

	IFloatAutomatableModel* midiCCModel(std::size_t index) override {
		return m_midiCCModel[index].get();
	}

	IInstrumentSoundShaping* soundShaping() override {
		return &m_soundShaping;
	}

	IInstrumentFunctionNoteStacking* noteStacking() override {
		return &m_noteStacking;
	}

	IInstrumentFunctionArpeggio* arpeggio() override {
		return &m_arpeggio;
	}

protected:
	QString nodeName() const override
	{
		return "instrumenttrack";
	}

	// get the name of the instrument in the saved data
	QString getSavedInstrumentName(const QDomElement & thisElement) const;

	InstrumentTrackModel* instrumentTrackModel() override {
		return &m_instrumentTrackModel;
	}

protected slots:
	void updateBaseNote();
	void updatePitch();
	void updatePitchRange();
	void updateMixerChannel();


private:
	void processCCEvent(int controller);

	InstrumentTrackModel m_instrumentTrackModel;

	MidiPort m_midiPort;

	NotePlayHandle* m_notes[NumKeys];
	NotePlayHandleList m_sustainedNotes;

	int m_runningMidiNotes[NumKeys];
	QMutex m_midiNotesMutex;

	bool m_sustainPedalPressed;

	bool m_silentBuffersProcessed;

	bool m_previewMode;

	IntModel m_baseNoteModel;	//!< The "A4" or "440 Hz" key (default 69)
	IntModel m_firstKeyModel;	//!< First key the instrument reacts to
	IntModel m_lastKeyModel;	//!< Last key the instrument reacts to

	bool m_hasAutoMidiDev;
	static InstrumentTrack *s_autoAssignedTrack;

	NotePlayHandleList m_processHandles;

	FloatModel m_volumeModel;
	FloatModel m_panningModel;

	AudioPort m_audioPort;

	FloatModel m_pitchModel;
	IntModel m_pitchRangeModel;
	IntModel m_mixerChannelModel;
	BoolModel m_useMasterPitchModel;

	IInstrument * m_instrument;
	InstrumentSoundShaping m_soundShaping;
	InstrumentFunctionArpeggio m_arpeggio;
	InstrumentFunctionNoteStacking m_noteStacking;

	Piano m_piano;

	Microtuner m_microtuner;

	std::unique_ptr<BoolModel> m_midiCCEnable;
	std::unique_ptr<FloatModel> m_midiCCModel[MidiControllerCount];

	friend class NotePlayHandle;

} ;



} // namespace lmms

#endif // LMMS_INSTRUMENT_TRACK_H
