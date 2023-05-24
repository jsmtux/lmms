#ifndef LMMS_UI_CLIP_VIEW_FACTORY_H
#define LMMS_UI_CLIP_VIEW_FACTORY_H

#include "IClip.h"

namespace lmms
{
namespace gui
{

class ClipView;
class TrackView;

class ClipViewFactory {
public:
    static ClipView* createClipView(TrackView* tv, IClip* clip);
};

} // namespace gui
} // namespace lmms

#endif /* LMMS_UI_CLIP_VIEW_FACTORY_H */
