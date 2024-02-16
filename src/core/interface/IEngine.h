#pragma once

#include "IAudioEngine.h"
#include "ISong.h"
#include "IProjectJournal.h"
#include "ITrack.h"
#include "IPatternStore.h"
#include "IMixer.h"
#include "IModels.h"

namespace lmms {

class IEngine : public QObject
{
    Q_OBJECT
public:
    static void InitIEngine(IEngine* engine);
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

    virtual IDetuningHelper* createDetuningHelper() = 0;

    virtual IAutomatableModel<float>* createFloatModel(float val, float min, float max, float step,
        QObject * parent,
        const QString& displayName) = 0;
    virtual IAutomatableModel<int>* createIntModel(int val, int min, int max, int step,
        QObject * parent,
        const QString& displayName) = 0;
    virtual IAutomatableModel<bool>* createBoolModel(bool val, bool min, bool max, bool step,
        QObject * parent,
        const QString& displayName) = 0;
    virtual IComboBoxModelWrapper* createComboBox(QObject* parent,
				const QString& displayName) = 0;
    virtual std::unique_ptr<ISampleBuffer> createSampleBuffer() = 0;
    virtual IHandleState* createHandleState(bool varyingPitch, int interpolationMode) = 0;
private:
    static IEngine* m_instance;
};


void setEngineDndPluginKey(PluginDescriptor::Key* newKey);

}