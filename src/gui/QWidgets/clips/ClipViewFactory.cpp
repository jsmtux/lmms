#include "ClipViewFactory.h"

#include "AutomationClipView.h"
#include "AutomationClip.h"
#include "PatternClipView.h"
#include "PatternClip.h"
#include "MidiClipView.h"
#include "MidiClip.h"
#include "SampleClipView.h"
#include "SampleClip.h"

namespace lmms
{
namespace gui
{

ClipView* ClipViewFactory::createClipView(TrackView* tv, Clip* clip) {
    ClipView* ret = nullptr;
    switch (clip->getType())
    {
    case ClipType::Automation:
        ret = new AutomationClipView(static_cast<AutomationClip*>(clip), tv);
        break;
    case ClipType::Pattern:
        ret = new PatternClipView(static_cast<PatternClip*>(clip), tv);
        break;    
    case ClipType::Midi:
        ret = new MidiClipView(static_cast<MidiClip*>(clip), tv);
        break;    
    case ClipType::Sample:
        ret = new SampleClipView(static_cast<SampleClip*>(clip), tv);
        break;
    }
    return ret;
}

} // namespace gui
} // namespace lmms