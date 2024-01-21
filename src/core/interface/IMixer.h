#pragma once

#include "IEffectChain.h"
#include "JournallingObject.h"

namespace lmms {

class IMixerChannel {
public:
    virtual ~IMixerChannel() = default;
    virtual const QString& getName() const = 0;
    virtual void setName(const QString& name) = 0;
    virtual IBoolAutomatableModel* getMutedModel() = 0;
    virtual void setColor (QColor newColor) = 0;
    virtual bool hasColor() = 0;
    virtual void removeColor() = 0;
    virtual QColor& getColor() = 0;
    virtual IFloatAutomatableModel* volumeModel() = 0;
    virtual IBoolAutomatableModel* muteModel() = 0;
    virtual IBoolAutomatableModel* soloModel() = 0;
    virtual IEffectChain* fxChain() = 0;
    virtual float peakLeft() = 0;
    virtual void setPeakLeft(float value) = 0;
    virtual float peakRight() = 0;
    virtual void setPeakRight(float value) = 0;
    virtual int channelIndex() const = 0;
};

class MixerRoute;

class IMixer : public JournallingObject {
public:
    virtual ~IMixer() = default;
    virtual IMixerChannel* getMixerChannelInterface(std::size_t channel) = 0;
    virtual IFloatAutomatableModel * channelSendModel(mix_ch_t fromChannel, mix_ch_t toChannel) = 0;
    virtual mix_ch_t numChannels() const = 0;
    virtual int createChannel() = 0;
    virtual void toggledSolo() = 0;
    virtual bool isInfiniteLoop(mix_ch_t fromChannel, mix_ch_t toChannel) = 0;
    virtual void clear() = 0;
    virtual void clearChannel(mix_ch_t channelIndex) = 0;
    virtual void deleteChannel(int index) = 0;
    virtual bool isChannelInUse(int index) = 0;
	virtual void moveChannelLeft(int index) = 0;
	virtual void moveChannelRight(int index) = 0;
    virtual MixerRoute * createChannelSend(mix_ch_t fromChannel, mix_ch_t toChannel,
						   float amount = 1.0f) = 0;
	virtual void deleteChannelSend(mix_ch_t fromChannel, mix_ch_t toChannel) = 0;
};

} // namespace lmms
