#include "IModels.h"
#include "IEngine.h"

namespace lmms {

template<>
IAutomatableModel<float>* MFact::create(float val, float min, float max, float step,
        QObject * parent,
        const QString& displayName){
	return IEngine::Instance()->createFloatModel(val, min, max, step, parent, displayName);
}
template<>
IAutomatableModel<int>* MFact::create(
        int val, int min, int max, int step,
        QObject * parent,
        const QString& displayName){
	return IEngine::Instance()->createIntModel(
        val, min, max, step,
        parent, displayName);
}
template<>
IAutomatableModel<bool>* MFact::create(
        bool val, bool min, bool max, bool step,
        QObject * parent,
        const QString& displayName){
	return IEngine::Instance()->createBoolModel(
        val, min, max, step, parent, displayName);
}

IAutomatableModel<int>* MFact::createIntModel(
	int val, int min, int max,
	QObject * parent,
	const QString& displayName) {
	return IEngine::Instance()->createIntModel(
        val, min, max, 1,
        parent, displayName);
}

IComboBoxModelWrapper* MFact::createComboBox(QObject* parent,
				const QString& displayName) {
	return IEngine::Instance()->createComboBox(parent, displayName);
}

} // namespace lmms
