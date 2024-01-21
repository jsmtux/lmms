#ifndef I_STEP_RECORDER_WIDGET_H_
#define I_STEP_RECORDER_WIDGET_H_

#include "TimePos.h"

namespace lmms::gui {

class IStepRecorderWidget {
public:
	virtual ~IStepRecorderWidget()
	{}
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void showHint() = 0;
	virtual void setStepsLength(TimePos stepsLength) = 0;
	virtual void setStartPosition(TimePos pos) = 0;
	virtual void setEndPosition(TimePos pos) = 0;
};

}

#endif
