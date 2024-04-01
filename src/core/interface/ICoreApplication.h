#pragma once

#include "IConfigManager.h"
#include "IAudioEngine.h"
#include "IRenderManager.h"
#include "OutputSettings.h"
#include "IDataFile.h"
#include "IEngine.h"

#include <memory>

namespace lmms {

class ICoreApplication {
public:
    virtual ~ICoreApplication() = default;
    virtual void init() = 0;
    virtual void free() = 0;

    virtual IConfigManager* getConfigManager() = 0;
    virtual void setNaNHandler( bool use ) = 0;
};

ICoreApplication* getCoreApplication();

}