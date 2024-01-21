#pragma once

#include <QObject>

#include "lmms_basics.h"

class QPainter;
class QRect;

namespace lmms
{

class ISampleBuffer {
public:
    virtual ~ISampleBuffer() = default;
    virtual const QString & audioFile() const = 0;
    virtual void loadFromBase64(const QString & data) = 0;
    virtual QString openAudioFile() const = 0;
    virtual f_cnt_t frames() const = 0;
    virtual bool reversed() const = 0;
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
};

std::unique_ptr<ISampleBuffer> createSampleBuffer();

} // namespace lmms
