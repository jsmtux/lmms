#include "ICoreApplication.h"
#include "NotePlayHandle.h"
#include "ConfigManager.h"
#include "DataFile.h"
#include "Engine.h"
#include "RenderManager.h"

namespace lmms {

class CoreApplication: public ICoreApplication
{
public:
    void init() override
    {
        NotePlayHandleManager::init();
    }

    void free() override
    {
        NotePlayHandleManager::free();
    }

    IConfigManager* getConfigManager() override
    {
        return ConfigManager::inst();
    }

    IRenderManager* createRenderManager(
		const IAudioEngine::qualitySettings & qualitySettings,
		const OutputSettings & outputSettings,
		IProjectRenderer::ExportFileFormats fmt,
		QString outputPath) override
    {
        return new RenderManager(qualitySettings, outputSettings, fmt, outputPath);
    }

    void setNaNHandler( bool use ) override
    {

    }

    std::unique_ptr<IEngine> createEngine(bool renderOnly) override
    {
        m_enginePointer = new Engine();
        return std::unique_ptr<IEngine>(m_enginePointer);
    }

    IEngine* getEngineInteface() override
    {
        return m_enginePointer;
    }
private:
    IEngine* m_enginePointer = nullptr;
};

LMMS_EXPORT ICoreApplication* getCoreApplication() {
    static CoreApplication coreApplication;
    return &coreApplication;
}

}
