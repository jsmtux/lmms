#include "IModels.h"
#include "IEngine.h"

namespace lmms {

IAutomatableModel<float>* MFact::create(
        float val, float min, float max, float step,
        QObject * parent,
        const QString& displayName){
        return IEngine::Instance()->createFloatModel(val, min, max, step, parent, displayName);
}

IAutomatableModel<bool>* MFact::create(
        bool val,
        QObject * parent,
        const QString& displayName){
	return IEngine::Instance()->createBoolModel(
        val, 0, 1, 0, parent, displayName);
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
