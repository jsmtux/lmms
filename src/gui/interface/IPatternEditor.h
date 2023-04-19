#ifndef I_PATTERN_EDITOR_H_
#define I_PATTERN_EDITOR_H_

class IPatternEditor {
public:
    virtual ~IPatternEditor() {}
    virtual void clearAllTracks() = 0;
};

#endif