#pragma once

#include <QObject>

#include "lmms_basics.h"

class QPainter;
class QRect;

namespace lmms
{

class IHandleState {
public:
    virtual ~IHandleState() = default;
    virtual void setFrameIndex(f_cnt_t index) = 0;
    virtual void setBackwards(bool backwards) = 0;
    virtual const f_cnt_t frameIndex() const = 0;
    virtual bool isBackwards() const = 0;
};
IHandleState* createHandleState(bool varyingPitch, int interpolationMode);

class ISampleBuffer {
public:
	enum LoopMode {
		LoopOff = 0,
		LoopOn,
		LoopPingPong
	};
    virtual ~ISampleBuffer() = default;
    virtual const QString & audioFile() const = 0;
    virtual void loadFromBase64(const QString & data) = 0;
    virtual QString openAudioFile() const = 0;
    virtual f_cnt_t frames() const = 0;
    virtual bool reversed() const = 0;
    virtual bool play(
		sampleFrame * ab,
		IHandleState * state_interface,
		const fpp_t frames,
		const float freq,
		const LoopMode loopMode = LoopOff
	) = 0;
    virtual void visualize(
		QPainter & p,
		const QRect & dr,
		const QRect & clip,
		f_cnt_t fromFrame = 0,
		f_cnt_t toFrame = 0
	) = 0;
    virtual void setReversed(bool on) = 0;
    virtual QString openAndSetWaveformFile() = 0;
    virtual void setAudioFile(const QString & audioFile) = 0;
    virtual void dataReadLock() = 0;
    virtual void dataUnlock() = 0;
    virtual sample_t userWaveSample(const float sample) const = 0;
    virtual QString & toBase64(QString & dst) const = 0;
    virtual f_cnt_t startFrame() const = 0;
    virtual f_cnt_t endFrame() const = 0;
    virtual void setAmplification(float a) = 0;
    virtual void setAllPointFrames(
		f_cnt_t start,
		f_cnt_t end,
		f_cnt_t loopStart,
		f_cnt_t loopEnd
	) = 0;
};
std::unique_ptr<ISampleBuffer> createSampleBuffer();

} // namespace lmms
