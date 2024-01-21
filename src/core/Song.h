/*
 * Song.h - class song - the root of the model-tree
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

#ifndef LMMS_SONG_H
#define LMMS_SONG_H

#include <memory>

#include <QHash>
#include <QString>

#include "ISong.h"

#include "TrackContainer.h"
#include "AudioEngine.h"
#include "lmms_constants.h"
#include "MeterModel.h"
#include "VstSyncController.h"
#include "Engine.h"
#include "Scale.h"
#include "Keymap.h"

namespace lmms
{

class AutomationTrack;
class Keymap;
class MidiClip;

namespace gui
{

class ITimeLineWidget;
class SongEditor;
class ControllerRackView;

}


class LMMS_EXPORT Song : public ISong, public TrackContainerCb
{
	Q_OBJECT
	mapPropertyFromModel( int,masterPitch,setMasterPitch,m_masterPitchModel );
	mapPropertyFromModel( int,masterVolume,setMasterVolume, m_masterVolumeModel );
public:
	struct SaveOptions : public ISaveOptions{
		IBoolAutomatableModel& getDiscardMIDIConnections() override {
			return discardMIDIConnections;
		}
		IBoolAutomatableModel& getSaveAsProjectBundle() override {
			return saveAsProjectBundle;
		}
		/**
		 * Should we discard MIDI ControllerConnections from project files?
		 */
		BoolModel discardMIDIConnections{false};
		/**
		 * Should we save the project as a project bundle? (with resources)
		 */
		BoolModel saveAsProjectBundle{false};

		void setDefaultOptions() {
			discardMIDIConnections.setValue(false);
			saveAsProjectBundle.setValue(false);
		}
	};

	void clearErrors();
	void collectError( const QString error ) override;
	bool hasErrors();
	QString errorSummary();

	void processNextBuffer();

	inline int getLoadingTrackCount() const
	{
		return m_nLoadingTrack;
	}

	inline int getMilliseconds() const override
	{
		return m_elapsedMilliSeconds[m_playMode];
	}

	inline int getMilliseconds(PlayModes playMode) const
	{
		return m_elapsedMilliSeconds[playMode];
	}

	inline void setToTime(TimePos const & pos)
	{
		m_elapsedMilliSeconds[m_playMode] = pos.getTimeInMilliseconds(getTempo());
		m_playPos[m_playMode].setTicks(pos.getTicks());
	}

	inline void setToTime(TimePos const & pos, PlayModes playMode) override
	{
		m_elapsedMilliSeconds[playMode] = pos.getTimeInMilliseconds(getTempo());
		m_playPos[playMode].setTicks(pos.getTicks());
	}

	inline void setToTimeByTicks(tick_t ticks)
	{
		m_elapsedMilliSeconds[m_playMode] = TimePos::ticksToMilliseconds(ticks, getTempo());
		m_playPos[m_playMode].setTicks(ticks);
	}

	inline void setToTimeByTicks(tick_t ticks, PlayModes playMode)
	{
		m_elapsedMilliSeconds[playMode] = TimePos::ticksToMilliseconds(ticks, getTempo());
		m_playPos[playMode].setTicks(ticks);
	}

	inline int getBars() const
	{
		return currentBar();
	}

	inline int ticksPerBar() const override
	{
		return TimePos::ticksPerBar(m_timeSigModel);
	}

	// Returns the beat position inside the bar, 0-based
	inline int getBeat() const
	{
		return getPlayPos().getBeatWithinBar(m_timeSigModel);
	}
	// the remainder after bar and beat are removed
	inline int getBeatTicks() const
	{
		return getPlayPos().getTickWithinBeat(m_timeSigModel);
	}
	inline int getTicks() const
	{
		return currentTick();
	}
	inline f_cnt_t getFrames() const
	{
		return currentFrame();
	}
	inline bool isPaused() const
	{
		return m_paused;
	}

	inline bool isPlaying() const override
	{
		return m_playing == true && m_exporting == false;
	}

	inline bool isStopped() const override
	{
		return m_playing == false && m_paused == false;
	}

	inline bool isExporting() const override
	{
		return m_exporting;
	}

	inline void setExportLoop( bool exportLoop ) override
	{
		m_exportLoop = exportLoop;
	}

	inline bool isRecording() const
	{
		return m_recording;
	}
	
	inline void setLoopRenderCount(int count) override
	{
		if (count < 1)
			m_loopRenderCount = 1;
		else
			m_loopRenderCount = count;
		m_loopRenderRemaining = m_loopRenderCount;
	}
    
	inline int getLoopRenderCount() const
	{
		return m_loopRenderCount;
	}

	bool isExportDone() const;
	int getExportProgress() const;

	inline void setRenderBetweenMarkers( bool renderBetweenMarkers ) override
	{
		m_renderBetweenMarkers = renderBetweenMarkers;
	}

	inline PlayModes playMode() const override
	{
		return m_playMode;
	}

	inline PlayPos & getPlayPos( PlayModes pm ) override
	{
		return m_playPos[pm];
	}
	inline const PlayPos & getPlayPos( PlayModes pm ) const
	{
		return m_playPos[pm];
	}
	inline PlayPos & getPlayPos() override
	{
		return getPlayPos(m_playMode);
	}
	inline const PlayPos & getPlayPos() const
	{
		return getPlayPos(m_playMode);
	}
	int getTempo() const override
	{
		return m_tempoModel.value();
	}
	
	void setTempo(int value) {
		m_tempoModel.setValue(value);
	}

	void updateLength();
	bar_t length() const
	{
		return m_length;
	}


	bpm_t getTempo();

	AutomationTrack * globalAutomationTrack()
	{
		return m_globalAutomationTrack;
	}

	//TODO: Add Q_DECL_OVERRIDE when Qt4 is dropped
	AutomatedValueMap automatedValuesAt(TimePos time, int clipNum = -1) const override;

	// file management
	void createNewProject() override;
	void createNewProjectFromTemplate( const QString & templ ) override;
	void loadProject( const QString & filename ) override;
	bool guiSaveProject() override;
	bool guiSaveProjectAs(const QString & filename) override;
	bool saveProjectFile(const QString & filename, bool withResources = false) override;

	const QString & projectFileName() const override
	{
		return m_fileName;
	}

	bool isLoadingProject() const override
	{
		return m_loadingProject;
	}

	void loadingCancelled()
	{
		m_isCancelled = true;
		Engine::audioEngine()->clearNewPlayHandles();
	}

	bool isCancelled()
	{
		return m_isCancelled;
	}

	bool isModified() const override
	{
		return m_modified;
	}

	QString nodeName() const override
	{
		return "song";
	}

	virtual bool fixedClips() const
	{
		return false;
	}

	void addController( IController * c ) override;
	void removeController( IController * c ) override;


	const ControllerVector & controllers() const override
	{
		return m_controllers;
	}


	MeterModel & getTimeSigModel()
	{
		return m_timeSigModel;
	}

	IMeterModel& getTimeSigModelInterface() override {
		return getTimeSigModel();
	}

	IntModel& tempoModel() override
	{
		return m_tempoModel;
	}

	IntModel& masterVolumeModel() override
	{
		return m_masterVolumeModel;
	}

	IntModel& masterPitchModel() override
	{
		return m_masterPitchModel;
	}

	int numOfPatterns() const override
	{
		return m_trackContainer.countTracks(Track::PatternTrack);
	}

	void setUpPatternStoreTrack() override {
		if (numOfPatterns() == 0 && !isLoadingProject()) {
			addPatternTrack();
		}
	}

	void exportProjectMidi(QString const & exportFileName) const override;

	inline void setLoadOnLaunch(bool value) override { m_loadOnLaunch = value; }
	SaveOptions &getSaveOptions() {
		return m_saveOptions;
	}
	virtual ISaveOptions &getSaveOptionsInterface() override {
		return m_saveOptions;
	}

	bool isSavingProject() const;

	const std::unique_ptr<Scale>& getScale(unsigned int index) const;
	const IScale* getScaleInterface(unsigned int index) const override {
		return getScale(index).get();
	}
	const std::unique_ptr<Keymap>& getKeymap(unsigned int index) const;
	const IKeymap* getKeymapInterface(unsigned int index) const override {
		return getKeymap(index).get();
	}
	void setScale(unsigned int index, IScale* newScale) override;
	void setKeymap(unsigned int index, IKeymap* newMap) override;

	const std::string& syncKey() const noexcept { return m_vstSyncController.sharedMemoryKey(); }

	ITrackContainer* trackContainerInterface() override {
		return &m_trackContainer;
	}

	const TrackContainer& trackContainer() const
	{
		return m_trackContainer;
	}
	ITrackContainer& trackContainer() override
	{
		return m_trackContainer;
	}

public slots:
	void playSong() override;
	void record() override;
	void playAndRecord() override;
	void playPattern() override;
	void playMidiClip( const lmms::IMidiClip * midiClipToPlay, bool loop = true ) override;
	void togglePause() override;
	void stop() override;

	void startExport();
	void stopExport();


	void setModified() override;

	void clearProject();

	void addPatternTrack();

	// now update all track-labels (the current one has to become white, the others gray)
	void updatePatternTracks()
	{
		for (ITrack * t : m_trackContainer.tracks())
		{
			if (t->type() == Track::PatternTrack)
			{
				t->model()->dataChanged();
			}
		}
	}


private slots:
	void insertBar() override;
	void removeBar() override;
	void addSampleTrack();
	void addAutomationTrack();

	void setTempo();
	void setTimeSignature();

	void masterVolumeChanged();

	void savePos();

	void updateFramesPerTick();

private:
	Song();
	Song( const Song & );
	virtual ~Song() override;

	TrackContainer m_trackContainer;

	inline bar_t currentBar() const
	{
		return m_playPos[m_playMode].getBar();
	}

	inline tick_t currentTick() const override
	{
		return m_playPos[m_playMode].getTicks();
	}

	inline f_cnt_t currentFrame() const
	{
		return m_playPos[m_playMode].getTicks() * Engine::framesPerTick() +
			m_playPos[m_playMode].currentFrame();
	}

	void setPlayPos( tick_t ticks, PlayModes playMode ) override;

	void saveControllerStates( QDomDocument & doc, QDomElement & element );
	void restoreControllerStates( const QDomElement & element );

	void removeAllControllers();

	void saveScaleStates(QDomDocument &doc, QDomElement &element);
	void restoreScaleStates(const QDomElement &element);

	void saveKeymapStates(QDomDocument &doc, QDomElement &element);
	void restoreKeymapStates(const QDomElement &element);

	void processAutomations(const TrackList& tracks, TimePos timeStart, fpp_t frames);

	void setModified(bool value);

	void setProjectFileName(QString const & projectFileName);

	AutomationTrack * m_globalAutomationTrack;

	IntModel m_tempoModel;
	MeterModel m_timeSigModel;
	int m_oldTicksPerBar;
	IntModel m_masterVolumeModel;
	IntModel m_masterPitchModel;

	ControllerVector m_controllers;

	int m_nLoadingTrack;

	QString m_fileName;
	QString m_oldFileName;
	bool m_modified;
	bool m_loadOnLaunch;

	volatile bool m_recording;
	volatile bool m_exporting;
	volatile bool m_exportLoop;
	volatile bool m_renderBetweenMarkers;
	volatile bool m_playing;
	volatile bool m_paused;

	bool m_savingProject;
	bool m_loadingProject;
	bool m_isCancelled;

	SaveOptions m_saveOptions;

	QHash<QString, int> m_errors;

	PlayModes m_playMode;
	PlayPos m_playPos[Mode_Count];
	bar_t m_length;

	const MidiClip* m_midiClipToPlay;
	bool m_loopMidiClip;

	double m_elapsedMilliSeconds[Mode_Count];
	tick_t m_elapsedTicks;
	bar_t m_elapsedBars;

	VstSyncController m_vstSyncController;
    
	int m_loopRenderCount;
	int m_loopRenderRemaining;
	TimePos m_exportSongBegin;
	TimePos m_exportLoopBegin;
	TimePos m_exportLoopEnd;
	TimePos m_exportSongEnd;
	TimePos m_exportEffectiveLength;

	std::unique_ptr<Scale> m_scales[MaxScaleCount];
	std::unique_ptr<Keymap> m_keymaps[MaxKeymapCount];

	AutomatedValueMap m_oldAutomatedValues;

	friend class Engine;
	friend class gui::SongEditor;
	friend class gui::ControllerRackView;

signals:
	void projectLoaded();
	void playbackStateChanged();
	void playbackPositionChanged();
	void lengthChanged( int bars );
	void tempoChanged( lmms::bpm_t newBPM );
	void timeSignatureChanged( int oldTicksPerBar, int ticksPerBar );
	void updateSampleTracks();
	void stopped();
	void modified();
	void projectFileNameChanged();
	void scaleListChanged(int index);
	void keymapListChanged(int index);
} ;


} // namespace lmms

#endif // LMMS_SONG_H
