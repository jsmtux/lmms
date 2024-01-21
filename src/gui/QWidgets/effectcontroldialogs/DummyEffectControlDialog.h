#ifndef LMMS_GUI_DUMMY_EFFECT_CONTROL_DIALOG_H
#define LMMS_GUI_DUMMY_EFFECT_CONTROL_DIALOG_H

#include "EffectControlDialog.h"

namespace lmms::gui
{

class Knob;


class DummyEffectControlDialog : public EffectControlDialog
{
public:
	DummyEffectControlDialog( EffectControls * _controls ) :
		EffectControlDialog( _controls )
	{
	}

} ;

} // namespace lmms::gui

#endif /* LMMS_GUI_DUMMY_EFFECT_CONTROL_DIALOG_H */
