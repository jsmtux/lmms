#ifndef A54C7905_44F7_4053_87F7_583D9E15B671
#define A54C7905_44F7_4053_87F7_583D9E15B671

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

#endif /* A54C7905_44F7_4053_87F7_583D9E15B671 */
