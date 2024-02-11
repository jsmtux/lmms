#include "IEngine.h"

namespace lmms {

IEngine* IEngine::m_instance = nullptr;

IEngine* IEngine::Instance() {
	return m_instance;
}

void IEngine::InitIEngine(IEngine* engine) {
    if (m_instance != nullptr) {
        throw new std::runtime_error("IEngine cannot be initialized twice\n");
    }
    m_instance = engine;
}

} // namespace lmms