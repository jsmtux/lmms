#pragma once

#include "IModels.h"

#include <QObject>

namespace lmms::gui
{

class LMMSPropertyBase : public QObject
{
	Q_OBJECT
public:
	template<typename ReceiverFunctionPointer>
	LMMSPropertyBase(IAutomatableModelBase* model, QObject* receiver, ReceiverFunctionPointer receiverFunction)
	{
		connect(model->model(), &Model::dataChanged, receiver, receiverFunction);
	}
};

#define EXPOSE_LMMS_PROPERTY(VALTYPE,PROPNAME,UNAME) \
  Q_PROPERTY(VALTYPE PROPNAME READ get_ ## UNAME WRITE set_ ## UNAME NOTIFY modelchanged_ ## UNAME); \
  VALTYPE get_ ## UNAME () const { return UNAME->value(); } \
  void set_ ## UNAME (VALTYPE val) { UNAME->setValue(val); } \
  LMMSPropertyBase model_ ## UNAME{UNAME, this, [this](){emit modelchanged_ ## UNAME();}}; \
  Q_SIGNAL void modelchanged_ ## UNAME(); \
  Q_PROPERTY(VALTYPE PROPNAME ## _max_value READ get_ ## PROPNAME ## _max CONSTANT) \
  VALTYPE get_ ## PROPNAME ## _max() { return UNAME->maxValue(); } \
  Q_PROPERTY(VALTYPE PROPNAME ## _min_value READ get_ ## PROPNAME ## _min CONSTANT) \
  VALTYPE get_ ## PROPNAME ## _min() { return UNAME->minValue(); }
}