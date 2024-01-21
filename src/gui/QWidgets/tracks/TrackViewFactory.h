#ifndef _TRACK_VIEW_FACTORY_H_
#define _TRACK_VIEW_FACTORY_H_

#include "ITrack.h"
#include "TrackView.h"

namespace lmms::gui {

class TrackViewFactory {
public:
    TrackView* createTrackView(ITrack * _t, TrackContainerView* _tc);
};

}

#endif /* _TRACK_VIEW_FACTORY_H_ */
