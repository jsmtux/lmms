#pragma once

#include <QString>

namespace lmms {

class ISong
{
public:
    virtual ~ISong() = default;
    virtual void loadProject( const QString & filename ) = 0;
    virtual bool isEmpty() const = 0;
    virtual void setExportLoop( bool exportLoop ) = 0;
};

}