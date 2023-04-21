#ifndef I_MIXERVIEW_H_
#define I_MIXERVIEW_H_

namespace lmms::gui {

class IMixerView {
public:
    virtual ~IMixerView() {}
    virtual void refreshDisplay() = 0;
};

}

#endif // I_MIXERVIEW_H_