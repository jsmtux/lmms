#ifndef _TRACK_VIEW_FACTORY_H_
#define _TRACK_VIEW_FACTORY_H_

#include "Track.h"

namespace lmms::gui {

class TrackViewFactory {
public:
    TrackView* createTrackView(Track * _t, TrackContainerView* _tc);
};

}

#endif /* _TRACK_VIEW_FACTORY_H_ */
