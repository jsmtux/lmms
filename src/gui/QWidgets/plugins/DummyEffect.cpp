#include "DummyEffect.h"

namespace lmms {

IEffect* InstantiateDummyEffect(IEffectChain* _parent, const QDomElement& originalPluginData) {
    return new DummyEffect(_parent, originalPluginData);
}

}