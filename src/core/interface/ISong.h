#pragma once

#include <QString>
#include "TimePos.h"
#include "IModels.h"
#include "ITrackContainer.h"
#include "IController.h"
#include "IScale.h"
#include "IKeymap.h"

namespace lmms {

namespace gui
{

class ITimeLineWidget;

}

const bpm_t MinTempo = 10;
const bpm_t DefaultTempo = 140;
const bpm_t MaxTempo = 999;
const tick_t MaxSongLength = 9999 * DefaultTicksPerBar;

class ISong : public QObject
{
	Q_OBJECT
public:
	enum PlayModes
	{
		Mode_None,
		Mode_PlaySong,
		Mode_PlayPattern,
		Mode_PlayMidiClip,
		Mode_PlayAutomationClip,
		Mode_Count
	} ;

	struct ISaveOptions {
		virtual IBoolAutomatableModel& getDiscardMIDIConnections() = 0;
		virtual IBoolAutomatableModel& getSaveAsProjectBundle() = 0;
	};

	class PlayPos : public TimePos
	{
	public:
		PlayPos( const int abs = 0 ) :
			TimePos( abs ),
			m_timeLine( nullptr ),
			m_currentFrame( 0.0f )
		{
		}
		inline void setCurrentFrame( const float f )
		{
			m_currentFrame = f;
		}
		inline float currentFrame() const
		{
			return m_currentFrame;
		}
		inline void setJumped( const bool jumped )
		{
			m_jumped = jumped;
		}
		inline bool jumped() const
		{
			return m_jumped;
		}
		gui::ITimeLineWidget * m_timeLine;

	private:
		float m_currentFrame;
		bool m_jumped;

	} ;

    virtual ~ISong() = default;
	virtual void createNewProject() = 0;
    virtual void loadProject( const QString & filename ) = 0;
    virtual void setExportLoop( bool exportLoop ) = 0;
    virtual void collectError( const QString error ) = 0;
	virtual void exportProjectMidi(QString const & exportFileName) const = 0;
	virtual void createNewProjectFromTemplate( const QString & templ ) = 0;
	virtual void playSong() = 0;
	virtual void record() = 0;
	virtual void playAndRecord() = 0;
	virtual void playPattern() = 0;
	virtual ITrackContainer* trackContainerInterface() = 0;
	virtual void removeController( IController * c ) = 0;
	virtual void addController( IController * c ) = 0;
	virtual ITrackContainer& trackContainer() = 0;
	virtual const QString & projectFileName() const = 0;
	virtual bool isModified() const = 0;
	virtual void setModified() = 0;
	virtual void stop() = 0;
	virtual ISaveOptions &getSaveOptionsInterface() = 0;
	virtual bool saveProjectFile(const QString & filename, bool withResources = false) = 0;
	virtual bool isPlaying() const = 0;
	virtual bool isStopped() const = 0;
	virtual PlayModes playMode() const = 0;
	virtual bool isExporting() const = 0;
	virtual bool isLoadingProject() const = 0;
	virtual bool guiSaveProject() = 0;
	virtual bool guiSaveProjectAs(const QString & filename) = 0;
	virtual void setLoadOnLaunch(bool value) = 0;
	virtual PlayPos & getPlayPos() = 0;
	virtual PlayPos & getPlayPos( PlayModes pm ) = 0;
	virtual IMeterModel& getTimeSigModelInterface() = 0;
	virtual int numOfPatterns() const = 0;
	virtual void setUpPatternStoreTrack() = 0;
	virtual IIntAutomatableModel& tempoModel() = 0;
	virtual IIntAutomatableModel& masterVolumeModel() = 0;
	virtual IIntAutomatableModel& masterPitchModel() = 0;
	virtual void setToTime(TimePos const & pos, PlayModes playMode) = 0;
	virtual void setScale(unsigned int index, IScale* newScale) = 0;
	virtual void setKeymap(unsigned int index, IKeymap* newMap) = 0;
	virtual const IScale* getScaleInterface(unsigned int index) const = 0;
	virtual const IKeymap* getKeymapInterface(unsigned int index) const = 0;
	virtual void insertBar() = 0;
	virtual void removeBar() = 0;
	virtual tick_t currentTick() const = 0;
	virtual void setPlayPos( tick_t ticks, ISong::PlayModes playMode ) = 0;
	virtual const ControllerVector & controllers() const = 0;
	virtual void setLoopRenderCount(int count) = 0;
	virtual void setRenderBetweenMarkers( bool renderBetweenMarkers ) = 0;
	virtual int getMilliseconds() const = 0;
	virtual int ticksPerBar() const = 0;
	virtual int getTempo() const = 0;

	virtual void togglePause() = 0;

	virtual void playMidiClip( const lmms::IMidiClip * midiClipToPlay, bool loop = true ) = 0;

signals:
	void controllerAdded( lmms::IController * );
	void controllerRemoved( lmms::IController * );
};

}