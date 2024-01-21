#pragma once

#include "IProjectRenderer.h"
#include "IAudioEngine.h"
#include "OutputSettings.h"

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
	virtual void abortProcessing() = 0;

signals:
	void finished();
};

std::unique_ptr<IRenderManager> createRenderManager(const IAudioEngine::qualitySettings & qualitySettings,
		const OutputSettings & outputSettings,
		IProjectRenderer::ExportFileFormats fmt,
		QString outputPath);

}