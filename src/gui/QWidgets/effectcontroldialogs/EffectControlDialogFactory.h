#ifndef EFFECT_CONTROL_DIALOG_FACTORY_H_
#define EFFECT_CONTROL_DIALOG_FACTORY_H_

#include "EffectControlDialog.h"
#include "EffectControls.h"

#include <functional>
#include <unordered_map>
#include <typeindex>

namespace lmms::gui {

class EffectControlDialogFactory{
public:
    typedef std::function<EffectControlDialog*(EffectControls*)> EffectControlDialogConstructor;
    EffectControlDialog* createEffectView(EffectControls* effectControls);
    template<typename EffectClass> void registerEffectView(EffectControlDialogConstructor createFunction) {
        m_effectControlDialogCreators[typeid(EffectClass)] = createFunction;
    }
private:
    std::unordered_map<std::type_index, EffectControlDialogConstructor> m_effectControlDialogCreators;
};

}

#endif /* EFFECT_CONTROL_DIALOG_FACTORY_H_ */
