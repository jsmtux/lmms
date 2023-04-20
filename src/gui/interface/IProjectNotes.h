#ifndef I_PROJECT_NOTES_H_
#define I_PROJECT_NOTES_H_

#include <QDomElement>
#include <QDomDocument>

namespace lmms::gui
{

class IProjectNotes {
public:
    virtual ~IProjectNotes() {}
    virtual void clear() = 0;
    virtual QString nodeName() const = 0;
    virtual void restoreState( const QDomElement & _this ) = 0;
    virtual QDomElement saveState( QDomDocument & _doc, QDomElement & _parent ) = 0;
};

}

#endif // I_PROJECT_NOTES_H_