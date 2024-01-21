#pragma once

#include <QString>

namespace lmms
{

class IInterval {
public:
    virtual ~IInterval() = default;
    virtual float getRatio() const = 0;
};

IInterval* createInterval(uint32_t numerator, uint32_t denominator);
IInterval* createInterval(float cents);

class IScale {
public:
    virtual ~IScale() = default;
    virtual QString getDescription() const = 0;
    virtual const std::vector<QString> getIntervalStrings() const = 0;
};

IScale* createScale(QString description, std::vector<IInterval*>&& intervals);

}
