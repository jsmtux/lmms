#ifndef I_AUTOMATION_EDITOR_H_
#define I_AUTOMATION_EDITOR_H_

#include "AutomationClip.h"

#include <QDomDocument>
#include <QDomElement>

namespace lmms::gui{

class IAutomationEditor{
public:
	virtual ~IAutomationEditor()
	{}
	virtual void updateAfterClipChange() = 0;
};

}

#endif // I_AUTOMATION_EDITOR_H_
