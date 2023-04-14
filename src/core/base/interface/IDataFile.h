#ifndef ADCB5C63_9C85_4119_8703_36DF8FE5DECC
#define ADCB5C63_9C85_4119_8703_36DF8FE5DECC

#include <QDomDocument>

class IDataFile : public QDomDocument {
public:
    virtual QDomElement& content() = 0;
};

#endif /* ADCB5C63_9C85_4119_8703_36DF8FE5DECC */
