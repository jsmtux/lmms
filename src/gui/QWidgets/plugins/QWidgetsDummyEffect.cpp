#include "QWidgetsDummyEffect.h"

namespace lmms {

IEffect* InstantiateDummyEffect(IEffectChain* _parent, const QDomElement& originalPluginData) {
    return new QWidgetsDummyEffect(_parent, originalPluginData);
}

}