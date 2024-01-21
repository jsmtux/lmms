#pragma once

#include <QString>

namespace lmms
{
class IKeymap {
public:
    virtual ~IKeymap() = default;
	virtual QString getDescription() const = 0;
    virtual int getDegree(int key) const = 0;
	virtual int getMiddleKey() const = 0;
	virtual int getFirstKey() const = 0;
	virtual int getLastKey() const = 0;
	virtual int getBaseKey() const = 0;
	virtual float getBaseFreq() const = 0;

    virtual const std::vector<int> &getMap() const = 0;
};

IKeymap* createKeyMap(
		QString description,
		std::vector<int> newMap,
		int newFirst,
		int newLast,
		int newMiddle,
		int newBaseKey,
		float newBaseFreq);

}