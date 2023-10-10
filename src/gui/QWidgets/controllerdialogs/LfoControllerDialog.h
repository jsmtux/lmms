#ifndef LMMS_GUI_LFO_CONTROLLER_DIALOG_H
#define LMMS_GUI_LFO_CONTROLLER_DIALOG_H

#include "ControllerDialog.h"
#include "LfoController.h"

namespace lmms::gui {

class LfoControllerDialog : public ControllerDialog
{
	Q_OBJECT
public:
	LfoControllerDialog( LfoController * _controller, QWidget * _parent );
	~LfoControllerDialog() override;


protected:
	void contextMenuEvent( QContextMenuEvent * _me ) override;

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

#endif /* LMMS_GUI_LFO_CONTROLLER_DIALOG_H */
