#pragma once

#include <QTextStream>
#include <QString>

namespace lmms {

class IDataFile
{
public:
    virtual ~IDataFile() = default;

	virtual void write( QTextStream& strm ) = 0;
	virtual bool writeFile(const QString& fn, bool withResources = false) = 0;
};

}