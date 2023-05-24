#pragma once

#include "Model.h"
#include "IModels.h"
#include "ISampleBuffer.h"

namespace lmms {

class IEnvelopeAndLfoParameters : public Model
{
    Q_OBJECT
public:
	enum LfoShapes
	{
		SineWave,
		TriangleWave,
		SawWave,
		SquareWave,
		UserDefinedWave,
		RandomWave,
		NumLfoShapes
	} ;

    IEnvelopeAndLfoParameters(QObject* parent) : Model(parent) {}
    virtual ~IEnvelopeAndLfoParameters() =  default;

    virtual IFloatAutomatableModel* predelayModel() = 0;
    virtual IFloatAutomatableModel* attackModel() = 0;
    virtual IFloatAutomatableModel* holdModel() = 0;
    virtual IFloatAutomatableModel* decayModel() = 0;
    virtual IFloatAutomatableModel* sustainModel() = 0;
    virtual IFloatAutomatableModel* releaseModel() = 0;
    virtual IFloatAutomatableModel* amountModel() = 0;
    virtual IFloatAutomatableModel* lfoPredelayModel() = 0;
    virtual IFloatAutomatableModel* lfoAttackModel() = 0;
    virtual ITempoSyncKnobModelWrapper* lfoSpeedModel() = 0;
    virtual IFloatAutomatableModel* lfoAmountModel() = 0;
    virtual IIntAutomatableModel* lfoWaveModel() = 0;
    virtual IBoolAutomatableModel* x100Model() = 0;
    virtual IBoolAutomatableModel* controlEnvAmountModel() = 0;
    virtual ISampleBuffer* userWave() = 0;
	virtual f_cnt_t lfoPredelayFrames() = 0;
	virtual f_cnt_t lfoOscillationFrames() = 0;
    virtual f_cnt_t lfoAttackFrames() = 0;
};

} // namesapce lmms
