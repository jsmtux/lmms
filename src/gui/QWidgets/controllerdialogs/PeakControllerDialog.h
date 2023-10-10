#ifndef LMMS_GUI_PEAK_CONTROLLER_DIALOG_H
#define LMMS_GUI_PEAK_CONTROLLER_DIALOG_H

#include "PeakController.h"
#include "ControllerDialog.h"

namespace lmms::gui {

class PeakControllerDialog : public ControllerDialog
{
	Q_OBJECT
public:
	PeakControllerDialog( Controller * _controller, QWidget * _parent );
	~PeakControllerDialog() override = default;

protected:
	void contextMenuEvent( QContextMenuEvent * _me ) override;
	void paintEvent( QPaintEvent * _pe ) override;

	PeakController * m_peakController;

};

}

#endif /* LMMS_GUI_PEAK_CONTROLLER_DIALOG_H */
