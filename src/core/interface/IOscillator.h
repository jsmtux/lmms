#pragma once

#include "lmms_basics.h"
#include "lmms_math.h"
#include "IModels.h"
#include "ISampleBuffer.h"

namespace lmms
{

class IOscillator
{
public:
	virtual ~IOscillator() = default;
	enum WaveShapes
	{
		SineWave,
		TriangleWave,
		SawWave,
		SquareWave,
		MoogSawWave,
		ExponentialWave,
		WhiteNoise,
		UserDefinedWave,
		NumWaveShapes,                                         //!< Number of all available wave shapes
		FirstWaveShapeTable = TriangleWave,                    //!< First wave shape that has a pre-generated table
		NumWaveShapeTables = WhiteNoise - FirstWaveShapeTable, //!< Number of band-limited wave shapes to be generated
	};

	enum ModulationAlgos
	{
		PhaseModulation,
		AmplitudeModulation,
		SignalMix,
		SynchronizedBySubOsc,
		FrequencyModulation,
		NumModulationAlgos
	} ;

	// now follow the wave-shape-routines...
	static inline sample_t sinSample( const float _sample )
	{
		return sinf( _sample * F_2PI );
	}

	static inline sample_t triangleSample( const float _sample )
	{
		const float ph = absFraction( _sample );
		if( ph <= 0.25f )
		{
			return ph * 4.0f;
		}
		else if( ph <= 0.75f )
		{
			return 2.0f - ph * 4.0f;
		}
		return ph * 4.0f - 4.0f;
	}

	static inline sample_t sawSample( const float _sample )
	{
		return -1.0f + absFraction( _sample ) * 2.0f;
	}

	static inline sample_t squareSample( const float _sample )
	{
		return ( absFraction( _sample ) > 0.5f ) ? -1.0f : 1.0f;
	}

	static inline sample_t noiseSample( const float )
	{
		// Precise implementation
//		return 1.0f - rand() * 2.0f / RAND_MAX;

		// Fast implementation
		return 1.0f - fast_rand() * 2.0f / FAST_RAND_MAX;
	}

	virtual void setUseWaveTable(bool n) = 0;
	virtual void setUserWave( const ISampleBuffer * _wave ) = 0;
	virtual void update(sampleFrame* ab, const fpp_t frames, const ch_cnt_t chnl, bool modulator = false) = 0;
};

IOscillator* createOscillator(const IIntAutomatableModel *wave_shape_model,
			const IIntAutomatableModel *mod_algo_model,
			const float &freq,
			const float &detuning_div_samplerate,
			const float &phase_offset,
			const float &volume,
			IOscillator *m_subOsc = nullptr);

} // namespace lmms