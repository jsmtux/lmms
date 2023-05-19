#ifndef LMMS_UI_CLIP_VIEW_FACTORY_H
#define LMMS_UI_CLIP_VIEW_FACTORY_H

#include "Clip.h"

namespace lmms
{
namespace gui
{

class ClipViewFactory {
public:
    static ClipView* createClipView(TrackView* tv, Clip* clip);
};

} // namespace gui
} // namespace lmms

#endif /* LMMS_UI_CLIP_VIEW_FACTORY_H */
