#include "ICoreApplication.h"
#include "NotePlayHandle.h"
#include "ConfigManager.h"
#include "DataFile.h"
#include "Engine.h"
#include "RenderManager.h"
#include "MixHelpers.h"

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

    void setNaNHandler( bool use ) override
    {
        MixHelpers::setNaNHandler(use);
    }
private:
    std::unique_ptr<IEngine> m_enginePointer;
};

LMMS_EXPORT ICoreApplication* getCoreApplication() {
    static CoreApplication coreApplication;
    return &coreApplication;
}

}
