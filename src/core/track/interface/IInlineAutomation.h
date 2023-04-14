#ifndef C4414F10_CF8F_4B5D_AC77_A109ADF0F48A
#define C4414F10_CF8F_4B5D_AC77_A109ADF0F48A

#include <QDomDocument>

namespace lmms {

// RTODO: actually create

class IInlineAutomation{
public:
    static IInlineAutomation* createDetuning() {return nullptr;}
    virtual ~IInlineAutomation() {}
    virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent ) = 0;
	virtual void loadSettings( const QDomElement & _this ) = 0;
    virtual AutomationClip * automationClip();
};

}

#endif /* C4414F10_CF8F_4B5D_AC77_A109ADF0F48A */
