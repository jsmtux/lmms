#ifndef Q_WIDGET_EFFECT_CONTROLS_H
#define Q_WIDGET_EFFECT_CONTROLS_H

#include "EffectControls.h"

namespace lmms::gui {

LMMS_EXPORT class QWidgetEffectControls : public EffectControls {
public:
    QWidgetEffectControls( Effect * _eff ) :
        EffectControls(_eff) {}
    virtual gui::EffectControlDialog * createView() = 0;
};

}

#endif /* Q_WIDGET_EFFECT_CONTROLS_H */
