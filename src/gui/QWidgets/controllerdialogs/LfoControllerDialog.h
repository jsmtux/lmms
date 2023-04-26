#ifndef EEE5B5F2_6BF1_48FE_8A4F_08A09D561CBE
#define EEE5B5F2_6BF1_48FE_8A4F_08A09D561CBE

#include "ControllerDialog.h"
#include "LfoController.h"

namespace lmms::gui {

class LfoControllerDialog : public ControllerDialog
{
	Q_OBJECT
public:
	LfoControllerDialog( Controller * _controller, QWidget * _parent );
	~LfoControllerDialog() override;


protected:
	void contextMenuEvent( QContextMenuEvent * _me ) override;
	void modelChanged() override;

	LfoController * m_lfo;

	Knob * m_baseKnob;
	TempoSyncKnob * m_speedKnob;
	Knob * m_amountKnob;
	Knob * m_phaseKnob;
	PixmapButton * m_userLfoBtn;
	automatableButtonGroup * m_waveBtnGrp;
	automatableButtonGroup * m_multiplierBtnGrp;


private:
	PixmapButton * m_userWaveBtn;

private slots:
	void askUserDefWave();

} ;

}

#endif /* EEE5B5F2_6BF1_48FE_8A4F_08A09D561CBE */
