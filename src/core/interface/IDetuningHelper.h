#pragma once

#include <QObject>

#include "shared_object.h"

namespace lmms
{

class IAutomationClip;

class IDetuningHelper :  public QObject, public sharedObject
{
    Q_OBJECT
public:
    virtual ~IDetuningHelper() {}
    virtual void setRange( const float min, const float max, const float step = 1 ) = 0;

    virtual IAutomationClip * automationClip() = 0;
    virtual bool hasAutomation() const = 0;
	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent ) = 0;
	virtual void loadSettings( const QDomElement & _this ) = 0;
};

} // namespace lmms
