#pragma once

#include <QString>

namespace lmms {

class IConfigManager {
public:
    virtual ~IConfigManager() = default;

    virtual void loadConfigFile(const QString & configFile = "") = 0;
    virtual QString localeDir() const = 0;
    virtual const QString & value(const QString & cls,
					const QString & attribute) const = 0;
	virtual const QString & value(const QString & cls,
					const QString & attribute,
					const QString & defaultVal) const = 0;
};

}