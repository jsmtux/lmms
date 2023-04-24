#include "TrackViewFactory.h"

#include "InstrumentTrackView.h"
#include "PatternTrackView.h"
#include "SampleTrackView.h"
#include "AutomationTrackView.h"

#include "GuiApplication.h"

#include "PatternTrack.h"
#include "SampleTrack.h"
#include "AutomationTrack.h"

namespace lmms::gui {

TrackView* TrackViewFactory::createTrackView(Track * _t, TrackContainerView* _tc) {
    switch(_t->type()) {
        case Track::InstrumentTrack:
            return new InstrumentTrackView(static_cast<InstrumentTrack*>(_t), _tc);
            break;
		case Track::PatternTrack:
            return new PatternTrackView(static_cast<PatternTrack*>(_t), _tc);
            break;
		case Track::SampleTrack:
            return new SampleTrackView(static_cast<SampleTrack*>(_t), _tc);
            break;
		case Track::AutomationTrack:
		case Track::HiddenAutomationTrack:
            return new AutomationTrackView(static_cast<AutomationTrack*>(_t), _tc);
            break;
        default:
            gui::getGUI()->mainWindow()->ShowCriticalMessage("Error creating track view", "No track view for track type " + QString(_t->type()));
            return nullptr;
    }
}

}