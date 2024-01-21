#include "ClipViewFactory.h"

#include "AutomationClipView.h"
#include "IClip.h"
#include "PatternClipView.h"
#include "MidiClipView.h"
#include "SampleClipView.h"

namespace lmms
{
namespace gui
{

ClipView* ClipViewFactory::createClipView(TrackView* tv, IClip* clip) {
    ClipView* ret = nullptr;
    auto* clipTypeSpecific = clip->getClipTypeSpecificInterface();
    switch (clip->getType())
    {
    case ClipType::Automation:
        ret = new AutomationClipView(static_cast<IAutomationClip*>(clipTypeSpecific), tv);
        break;
    case ClipType::Pattern:
        ret = new PatternClipView(static_cast<IPatternClip*>(clipTypeSpecific), tv);
        break;    
    case ClipType::Midi:
        ret = new MidiClipView(static_cast<IMidiClip*>(clipTypeSpecific), tv);
        break;    
    case ClipType::Sample:
        ret = new SampleClipView(static_cast<ISampleClip*>(clipTypeSpecific), tv);
        break;
    }
    return ret;
}

} // namespace gui
} // namespace lmms