#ifndef BA9C9C5D_CA42_49E5_AB0D_376C079C0180
#define BA9C9C5D_CA42_49E5_AB0D_376C079C0180

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

#endif /* BA9C9C5D_CA42_49E5_AB0D_376C079C0180 */
