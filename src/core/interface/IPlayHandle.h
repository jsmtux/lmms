/*
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef LMMS_I_PLAY_HANDLE_H
#define LMMS_I_PLAY_HANDLE_H

#include <QString>

#include "lmms_basics.h"

class QThread;

namespace lmms {

class IDataFile;
class IAudioPort;
class ITrack;
class ThreadableJob;

enum class PlayHandleType {
	TypeNotePlayHandle,
	TypeInstrumentPlayHandle,
	TypeSamplePlayHandle,
	TypePresetPreviewHandle
};

class IPlayHandle {
public:
    virtual ~IPlayHandle() = default;
	virtual PlayHandleType getType() = 0;
	virtual bool isFinished() const = 0;

	// Only used from AudioPort
	virtual bool usesBuffer() const = 0;
	virtual sampleFrame * buffer() = 0;
};

class INotePlayHandle : public virtual IPlayHandle {
public:
    virtual ~INotePlayHandle() = default;
	PlayHandleType getType() {
		return PlayHandleType::TypeNotePlayHandle;
	}
    virtual f_cnt_t totalFramesPlayed() const = 0;
    virtual fpp_t framesLeftForCurrentPeriod() const = 0;
    virtual f_cnt_t offset() const = 0;
    virtual fpp_t fadeInLength() = 0;
    virtual void setFadeInLength(fpp_t len) = 0;
    virtual f_cnt_t framesLeft() const = 0;
	virtual f_cnt_t noteOffset() const = 0;
	virtual void* pluginData() = 0;
	virtual void setPluginData(void* data) = 0;
	virtual const float& frequency() const = 0;
	virtual bool isReleased() const = 0;
	virtual void noteOff( const f_cnt_t offset = 0 ) = 0;
	virtual float releaseFramesDone() = 0;
};

INotePlayHandle* createNotePlayHandle(const QString& sampleFile);


class ISamplePlayHandle : public virtual IPlayHandle {
public:
    virtual ~ISamplePlayHandle() = default;
	PlayHandleType getType() {
		return PlayHandleType::TypeSamplePlayHandle;
	}
    virtual void setDoneMayReturnTrue( bool _enable ) = 0;
};

ISamplePlayHandle* createSamplePlayHandle(const QString& sampleFile);


class IInstrumentPlayHandle : public virtual IPlayHandle {
public:
    virtual ~IInstrumentPlayHandle() = default;
	PlayHandleType getType() {
		return PlayHandleType::TypeInstrumentPlayHandle;
	}
};

IInstrumentPlayHandle* createInstrumentPlayHandle(const QString& sampleFile);


class IPresetPreviewPlayHandle : public virtual IPlayHandle {
public:
    virtual ~IPresetPreviewPlayHandle() = default;
	PlayHandleType getType() override {
		return PlayHandleType::TypePresetPreviewHandle;
	}
};

IPresetPreviewPlayHandle* createPresetPreviewPlayHandle(const QString& presetFile, bool loadByPlugin, IDataFile* dataFile);

}

#endif // LMMS_I_PLAY_HANDLE_H