#ifndef INSTRUMENT_FACTORY_H_
#define INSTRUMENT_FACTORY_H_

#include <functional>
#include <unordered_map>
#include <typeindex>

class InstrumentView;


class InstrumentFactory {
public:
    typedef std::function<InstrumentView*(EffectControls*)> EffectControlDialogConstructor;
    EffectControlDialog* createEffectView(EffectControls* effectControls);
};

#endif /* INSTRUMENT_FACTORY_H_ */
