#include "ControllerDialogsFactory.h"

#include "LfoControllerDialog.h"
#include "PeakControllerDialog.h"

namespace lmms::gui {

ControllerDialog* ControllerDialogsFactory::createControllerDialog(Controller * _controller, QWidget * _parent ) {
    ControllerDialog* ret = nullptr;

    switch(_controller->type()) {
        case Controller::LfoController:
            ret = new LfoControllerDialog(_controller, _parent);
            break;
        case Controller::PeakController:
            ret = new PeakControllerDialog(_controller, _parent);
            break;
        default:
            break;
    }

    return ret;
}

}
