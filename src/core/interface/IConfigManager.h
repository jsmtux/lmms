#pragma once

#include <QString>
#include <QObject>

#include "lmmsconfig.h"
#include "lmms_basics.h"

namespace lmms {

const fpp_t MINIMUM_BUFFER_SIZE = 32;
const fpp_t DEFAULT_BUFFER_SIZE = 256;

class IConfigManager  : public QObject
{
	Q_OBJECT
public:
    virtual ~IConfigManager() = default;

    virtual void loadConfigFile(const QString & configFile = "") = 0;
    virtual QString localeDir() const = 0;
    virtual const QString & value(const QString & cls,
					const QString & attribute) const = 0;
	virtual const QString & value(const QString & cls,
					const QString & attribute,
					const QString & defaultVal) const = 0;
    virtual void setValue(const QString & cls, const QString & attribute,
						const QString & value) = 0;
    virtual const QString & workingDir() const = 0;
    virtual void createWorkingDir() = 0;
    virtual bool hasWorkingDir() const = 0;
    virtual const QString & dataDir() const = 0;
    virtual QString themeDir() const = 0;
    virtual QString defaultThemeDir() const = 0;
    virtual QString userProjectsDir() const = 0;
    virtual QString factoryProjectsDir() const = 0;
    virtual QString userSamplesDir() const = 0;
    virtual QString factorySamplesDir() const = 0;
    virtual QString userPresetsDir() const = 0;
    virtual QString factoryPresetsDir() const = 0;
    virtual const QString & backgroundPicFile() const = 0;
    virtual QString userTemplateDir() const = 0;
    virtual const QString recoveryFile() const = 0;
    virtual void addRecentlyOpenedProject(const QString & _file) = 0;
    virtual const QStringList & recentlyOpenedProjects() const = 0;
    virtual QString vstEmbedMethod() const = 0;
    virtual void setVSTDir(const QString & vstDir) = 0;
    virtual void setLADSPADir(const QString & ladspaDir) = 0;
    virtual const QString & gigDir() const = 0;
    virtual const QString & sf2Dir() const = 0;
	virtual const QString & vstDir() const = 0;
    virtual const QString & ladspaDir() const = 0;
    virtual QStringList getAvailableVstEmbedMethods() = 0;
    virtual void setWorkingDir(const QString & workingDir) = 0;
	virtual void setSF2Dir(const QString & sf2Dir) = 0;
	virtual void setSF2File(const QString & sf2File) = 0;
    virtual void setGIGDir(const QString & gigDir) = 0;
	virtual void setThemeDir(const QString & themeDir) = 0;
	virtual void setBackgroundPicFile(const QString & backgroundPicFile) = 0;
    virtual void saveConfigFile() = 0;
#ifdef LMMS_HAVE_FLUIDSYNTH
	virtual const QString & sf2File() const = 0;
#endif
    static IConfigManager* Instance();

signals:
	void valueChanged( QString cls, QString attribute, QString value );
};

}