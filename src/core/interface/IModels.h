#ifndef LMMS_I_MODELS_H
#define LMMS_I_MODELS_H

#include <QDomDocument>
#include <QDomElement>

#include <memory>

namespace lmms
{

template<typename T> class IAutomatableModel
{
public:
    virtual T value( int frameOffset = 0 ) const = 0;

	virtual void saveSettings( QDomDocument& doc, QDomElement& element, const QString& name ) = 0;
    virtual void loadSettings( const QDomElement& element, const QString& name ) = 0;
};

}

#endif /* LMMS_I_MODELS_H */
