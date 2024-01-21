#pragma once

#include "lmms_basics.h"
#include "IPlayHandle.h"
#include "IMidiClient.h"

#include <QString>
#include <QObject>

namespace lmms {

class IAudioDevice;

/*
** The following enums can be used to set the interpolator type
** using the function src_set_converter().
*/

enum
{
	SRC_SINC_BEST_QUALITY		= 0,
	SRC_SINC_MEDIUM_QUALITY		= 1,
	SRC_SINC_FASTEST			= 2,
	SRC_ZERO_ORDER_HOLD			= 3,
	SRC_LINEAR					= 4,
} ;

class IAudioEngine : public QObject
{
	Q_OBJECT
public:struct qualitySettings
	{
		enum Mode
		{
			Mode_Draft,
			Mode_HighQuality,
			Mode_FinalMix
		} ;

		enum Interpolation
		{
			Interpolation_Linear,
			Interpolation_SincFastest,
			Interpolation_SincMedium,
			Interpolation_SincBest
		} ;

		enum Oversampling
		{
			Oversampling_None,
			Oversampling_2x,
			Oversampling_4x,
			Oversampling_8x
		} ;

		Interpolation interpolation;
		Oversampling oversampling;

		qualitySettings(Mode m)
		{
			switch (m)
			{
				case Mode_Draft:
					interpolation = Interpolation_Linear;
					oversampling = Oversampling_None;
					break;
				case Mode_HighQuality:
					interpolation =
						Interpolation_SincFastest;
					oversampling = Oversampling_2x;
					break;
				case Mode_FinalMix:
					interpolation = Interpolation_SincBest;
					oversampling = Oversampling_8x;
					break;
			}
		}

		qualitySettings(Interpolation i, Oversampling o) :
			interpolation(i),
			oversampling(o)
		{
		}

		int sampleRateMultiplier() const
		{
			switch( oversampling )
			{
				case Oversampling_None: return 1;
				case Oversampling_2x: return 2;
				case Oversampling_4x: return 4;
				case Oversampling_8x: return 8;
			}
			return 1;
		}

		int libsrcInterpolation() const
		{
			switch( interpolation )
			{
				case Interpolation_Linear:
					return SRC_ZERO_ORDER_HOLD;
				case Interpolation_SincFastest:
					return SRC_SINC_FASTEST;
				case Interpolation_SincMedium:
					return SRC_SINC_MEDIUM_QUALITY;
				case Interpolation_SincBest:
					return SRC_SINC_BEST_QUALITY;
			}
			return SRC_LINEAR;
		}
	} ;


	struct StereoSample
	{
		StereoSample(sample_t _left, sample_t _right) : left(_left), right(_right) {}
		sample_t left;
		sample_t right;
	};

    virtual ~IAudioEngine() = default;

    virtual void setProfilerOutputFile(QString path) = 0;

	virtual fpp_t framesPerPeriod() const = 0;
	virtual sample_rate_t processingSampleRate() const = 0;
	virtual const qualitySettings& currentQualitySettings() const = 0;
	virtual void requestChangeInModel() = 0;
	virtual void doneChangeInModel() = 0;
	virtual bool addPlayHandle( IPlayHandle* handle ) = 0;
	virtual void removePlayHandle( IPlayHandle* handle ) = 0;
	virtual inline bool isMetronomeActive() const = 0;
	virtual bool audioDevStartFailed() const = 0;
	virtual bool isAudioDevNameValid(QString name) = 0;
	virtual bool isMidiDevNameValid(QString name) = 0 ;
	virtual inline void setMetronomeActive(bool value = true) = 0;
	virtual float masterGain() const = 0;
	virtual void changeQuality(const struct IAudioEngine::qualitySettings & qs) = 0;
	virtual void setMasterGain(const float mo) = 0;
	virtual IAudioDevice* audioDeviceInterface() = 0;
	virtual sample_rate_t baseSampleRate() const = 0;
	virtual IMidiClient * midiClientInterface() = 0;
	virtual int cpuLoad() const = 0;
	virtual StereoSample getPeakValues(sampleFrame * ab, const f_cnt_t _frames) const = 0;
	virtual const QString & audioDevName() const = 0;
	virtual const QString & midiClientName() const = 0;


	static inline sample_t clip(const sample_t s)
	{
		if (s > 1.0f)
		{
			return 1.0f;
		}
		else if (s < -1.0f)
		{
			return -1.0f;
		}
		return s;
	}
signals:
	void nextAudioBuffer( const lmms::surroundSampleFrame * buffer );
};

}