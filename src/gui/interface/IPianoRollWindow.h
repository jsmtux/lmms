#ifndef I_PIANO_ROLL_WINDOW_H_
#define I_PIANO_ROLL_WINDOW_H_

#include <QDomElement>
#include <QDomDocument>

namespace lmms {

class MidiClip;

namespace gui {

class IPianoRollWindow {
public:
    virtual ~IPianoRollWindow()
    {}
    virtual int quantization() const = 0;
    virtual const MidiClip* currentMidiClip() const = 0;
    virtual void update() = 0;
};

}
}

#endif
