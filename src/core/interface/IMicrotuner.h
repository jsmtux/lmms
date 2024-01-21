#pragma once

#include "IModels.h"

namespace lmms
{

class IMicrotuner {
public:
    virtual ~IMicrotuner() = default;
    virtual IComboBoxModelWrapper *keymapModel() = 0;
    virtual IComboBoxModelWrapper *scaleModel() = 0;
    virtual IBoolAutomatableModel *keyRangeImportModel() = 0;
    virtual IBoolAutomatableModel *enabledModel() = 0;
    virtual bool enabled() const = 0;
    virtual int octaveSize() const = 0;
};

}
