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

    std::unique_ptr<IDataFile> createDataFile(QString path) override
    {
        return std::make_unique<DataFile>(path);
    }

    void setNaNHandler( bool use ) override
    {

    }

    std::unique_ptr<IEngine> createEngine(bool renderOnly) override
    {
        return std::make_unique<Engine>();
    }
};

LMMS_EXPORT ICoreApplication* getCoreApplication() {
    static CoreApplication coreApplication;
    return &coreApplication;
}

}
