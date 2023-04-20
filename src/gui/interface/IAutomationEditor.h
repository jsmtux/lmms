#ifndef I_AUTOMATION_EDITOR_H_
#define I_AUTOMATION_EDITOR_H_

#include "AutomationClip.h"

#include <QDomDocument>
#include <QDomElement>

namespace lmms::gui{

class IAutomationEditor{
public:
    virtual ~IAutomationEditor() {}
    virtual void setCurrentClip(AutomationClip * new_clip) = 0;
    virtual void updateAfterClipChange() = 0;
    virtual QString nodeName() = 0;
	virtual QDomElement saveState( QDomDocument & _doc, QDomElement & _parent ) = 0;
	virtual void restoreState( const QDomElement & _this ) = 0;

};

}

#endif // I_AUTOMATION_EDITOR_H_