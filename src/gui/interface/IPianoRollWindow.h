#ifndef I_PIANO_ROLL_WINDOW_H_
#define I_PIANO_ROLL_WINDOW_H_

#include <QDomElement>
#include <QDomDocument>

namespace lmms {

class IMidiClip;

namespace gui {

class IPianoRollWindow {
public:
    virtual ~IPianoRollWindow()
    {}
    virtual int quantization() const = 0;
    virtual const IMidiClip* currentMidiClip() const = 0;
    virtual void update() = 0;
};

}
}

#endif
