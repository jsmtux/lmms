#pragma once

namespace lmms {

class IPatternStore {
public:
    virtual ~IPatternStore() = default;
    virtual ITrackContainer& trackContainer() = 0;
    virtual bar_t lengthOfPattern(int pattern) const = 0;
    virtual int currentPattern() const = 0;
    virtual void setCurrentPattern( const int val ) = 0;
    virtual bool play(TimePos start, const fpp_t frames, const f_cnt_t frameBase, int clipNum = -1) = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual IComboBoxModelWrapper* patternComboboxModel() = 0;
};

}