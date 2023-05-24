#pragma once

#include "IProjectRenderer.h"

#include <QObject>

namespace lmms {

class IRenderManager : public QObject
{
	Q_OBJECT
public:
    virtual ~IRenderManager() = default;

    virtual IProjectRenderer* getProjectRenderer() = 0;
    virtual void renderTracks() = 0;
    virtual void renderProject() = 0;

signals:
	void finished();

};

}