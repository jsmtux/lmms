#ifndef LMMS_GUI_CONTROLLER_DIALOGS_FACTORY_H
#define LMMS_GUI_CONTROLLER_DIALOGS_FACTORY_H

#include "IController.h"
#include "ControllerDialog.h"

namespace lmms::gui {

class ControllerDialogsFactory {
public:
    ControllerDialog* createControllerDialog(IController * _controller, QWidget * _parent );
};

}

#endif /* LMMS_GUI_CONTROLLER_DIALOGS_FACTORY_H */
