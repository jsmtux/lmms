#ifndef I_SONG_EDITOR_H_
#define I_SONG_EDITOR_H_

namespace lmms::gui {

class ISongEditor {
public:
    virtual ~ISongEditor() {}
    virtual void clearAllTracks() = 0;
};

}

#endif