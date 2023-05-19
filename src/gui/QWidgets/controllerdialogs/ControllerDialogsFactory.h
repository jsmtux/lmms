#ifndef LMMS_GUI_CONTROLLER_DIALOGS_FACTORY_H
#define LMMS_GUI_CONTROLLER_DIALOGS_FACTORY_H

#include "Controller.h"

namespace lmms::gui {

class ControllerDialogsFactory {
public:
    ControllerDialog* createControllerDialog(Controller * _controller, QWidget * _parent );
};

}

#endif /* LMMS_GUI_CONTROLLER_DIALOGS_FACTORY_H */
