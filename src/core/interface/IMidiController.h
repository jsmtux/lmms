#pragma once

#include <memory>

#include "IMidiPort.h"

namespace lmms
{

class ISong;
class IMidiPort;
class Model;

class IAutodetectMidiController;

class IMidiController {
public:
    virtual ~IMidiController() = default;
    virtual IMidiPort* midiPortInterface() = 0;
    virtual void subscribeReadablePorts( const IMidiPort::Map & _map ) = 0;
    virtual IController* controllerInterface() = 0;
    virtual IMidiController* copyToMidiController( Model* parent ) = 0;
    virtual IController* baseController() = 0;
    virtual IAutodetectMidiController* getAutodetectIfExists() { return nullptr; }
};

IMidiController* getMidiControllerFromBaseController(IController*);

class IAutodetectMidiController {
public:
    virtual void reset() = 0;
    virtual void useDetected() = 0;
};

std::unique_ptr<IMidiController> createAutoDetectMidiController(ISong* song);

}