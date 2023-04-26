#ifndef C42BE371_6AA1_4AB9_8D37_9047F0153F50
#define C42BE371_6AA1_4AB9_8D37_9047F0153F50

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
	void modelChanged() override;

	PeakController * m_peakController;

};

}

#endif /* C42BE371_6AA1_4AB9_8D37_9047F0153F50 */
