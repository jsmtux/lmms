#include "TrackViewFactory.h"

#include "ITrack.h"
#include "AutomationTrackView.h"
#include "GuiApplication.h"
#include "InstrumentTrackView.h"
#include "PatternTrackView.h"
#include "SampleTrackView.h"

namespace lmms::gui {

TrackView* TrackViewFactory::createTrackView(ITrack * _t, TrackContainerView* _tc) {
    switch(_t->type()) {
        case ITrack::InstrumentTrack:
            return new InstrumentTrackView(static_cast<IInstrumentTrack*>(_t->getTrackTypeSpecificInterface()), _tc);
            break;
		case ITrack::PatternTrack:
            return new PatternTrackView(static_cast<IPatternTrack*>(_t->getTrackTypeSpecificInterface()), _tc);
            break;
		case ITrack::SampleTrack:
            return new SampleTrackView(static_cast<ISampleTrack*>(_t->getTrackTypeSpecificInterface()), _tc);
            break;
		case ITrack::AutomationTrack:
		case ITrack::HiddenAutomationTrack:
            return new AutomationTrackView(static_cast<IAutomationTrack*>(_t->getTrackTypeSpecificInterface()), _tc);
            break;
        default:
            gui::getGUI()->mainWindow()->ShowCriticalMessage("Error creating track view", "No track view for track type " + QString(_t->type()));
            return nullptr;
    }
}

}
