#pragma once

#include "IAudioEngine.h"
#include "ISong.h"
#include "IProjectJournal.h"
#include "ITrack.h"
#include "IPatternStore.h"
#include "IMixer.h"

namespace lmms {

class IEngine : public QObject
{
    Q_OBJECT
public:
    static IEngine* Instance();
    virtual ~IEngine() = default;
    virtual ISong* getSongInterface() = 0;
    virtual IAudioEngine* getAudioEngineInterface() = 0;
    virtual IProjectJournal* getProjectJournalInterface() = 0;
    virtual IPatternStore* getPatternStoreInterface() = 0;
    virtual IMixer* getMixerInterface() = 0;
    virtual PluginDescriptor::Key* pickDndPluginKey() = 0;
    virtual TrackList getITracks(bool includeGlobalAutomation = false) = 0;
    virtual float getFramesPerTick() = 0;
};


void setEngineDndPluginKey(PluginDescriptor::Key* newKey);

void InitializeEngine(bool renderOnly);
void DestroyEngine();

}