#include "DetuningHelper.h"

namespace lmms
{

IDetuningHelper* createDetuningHelper() {
    return new DetuningHelper();
}


} // namespace lmms