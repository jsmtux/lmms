#ifndef CONTROLLER_DIALOGS_FACTORY_H_
#define CONTROLLER_DIALOGS_FACTORY_H_

#include "Controller.h"

namespace lmms::gui {

class ControllerDialogsFactory {
public:
    ControllerDialog* createControllerDialog(Controller * _controller, QWidget * _parent );
};

}

#endif /* CONTROLLER_DIALOGS_FACTORY_H_ */
