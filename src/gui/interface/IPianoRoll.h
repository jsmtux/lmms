#ifndef I_PIANO_ROLL_H_
#define I_PIANO_ROLL_H_

namespace lmms::gui {

class IPianoRoll {
public:
    virtual ~IPianoRoll()
    {}
    virtual int quantization() const = 0;
    virtual void update() = 0;
};

}

#endif // I_PIANO_ROLL_H_
