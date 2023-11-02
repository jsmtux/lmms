/*
 * AudioEngine.h - device-independent audio engine for LMMS
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_AUDIO_ENGINE_H
#define LMMS_AUDIO_ENGINE_H

#include <QMutex>

#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
	#include <QRecursiveMutex>
#endif

#include <QThread>
#include <QVector>
#include <QWaitCondition>
#include <samplerate.h>
#include <set>

#include "lmms_export.h"
#include "lmms_basics.h"
#include "LocklessList.h"
#include "FifoBuffer.h"
#include "AudioEngineProfiler.h"
#include "PlayHandle.h"
#include "IAudioEngine.h"
#include "Track.h"
#include "audio/AudioDevice.h"


namespace lmms
{

class AudioDevice;
class MidiClient;
class AudioPort;
class AudioEngineWorkerThread;


const int BYTES_PER_SAMPLE = sizeof( sample_t );
const int BYTES_PER_INT_SAMPLE = sizeof( int_sample_t );
const int BYTES_PER_FRAME = sizeof( sampleFrame );
const int BYTES_PER_SURROUND_FRAME = sizeof( surroundSampleFrame );

const float OUTPUT_SAMPLE_MULTIPLIER = 32767.0f;

class LMMS_EXPORT AudioEngine : public IAudioEngine
{
	Q_OBJECT
public:
	/**
	 * @brief RAII helper for requestChangesInModel.
	 * Used by AudioEngine::requestChangesGuard.
	 */
	class RequestChangesGuard {
		friend class AudioEngine;

	private:
		RequestChangesGuard(AudioEngine* audioEngine)
			: m_audioEngine{audioEngine}
		{
			m_audioEngine->requestChangeInModel();
		}
	public:

		RequestChangesGuard()
			: m_audioEngine{nullptr}
		{
		}

		RequestChangesGuard(RequestChangesGuard&& other)
			: RequestChangesGuard()
		{
			std::swap(other.m_audioEngine, m_audioEngine);
		}

		// Disallow copy.
		RequestChangesGuard(const RequestChangesGuard&) = delete;
		RequestChangesGuard& operator=(const RequestChangesGuard&) = delete;

		~RequestChangesGuard() {
			if (m_audioEngine) {
				m_audioEngine->doneChangeInModel();
			}
		}

	private:
		AudioEngine* m_audioEngine;
	};

	void initDevices();
	void clear();
	void clearNewPlayHandles();


	// audio-device-stuff

	// Returns the current audio device's name. This is not necessarily
	// the user's preferred audio device, in case you were thinking that.
	inline const QString & audioDevName() const override
	{
		return m_audioDevName;
	}
	inline bool audioDevStartFailed() const override
	{
		return m_audioDevStartFailed;
	}

	//! Set new audio device. Old device will be deleted,
	//! unless it's stored using storeAudioDevice
	void setAudioDevice( AudioDevice * _dev,
				const struct IAudioEngine::qualitySettings & _qs,
				bool _needs_fifo,
				bool startNow );
	void storeAudioDevice();
	void restoreAudioDevice();
	inline AudioDevice * audioDev()
	{
		return m_audioDev;
	}

	IAudioDevice* audioDeviceInterface() override {
		return audioDev();
	}


	// audio-port-stuff
	inline void addAudioPort(AudioPort * port)
	{
		requestChangeInModel();
		m_audioPorts.push_back(port);
		doneChangeInModel();
	}

	void removeAudioPort(AudioPort * port);


	// MIDI-client-stuff
	inline const QString & midiClientName() const override
	{
		return m_midiClientName;
	}

	inline MidiClient * midiClient()
	{
		return m_midiClient;
	}

	IMidiClient * midiClientInterface() override;


	// play-handle stuff
	bool addPlayHandle( IPlayHandle* handle ) override;

	void removePlayHandle( IPlayHandle* handle ) override;

	inline PlayHandleList& playHandles()
	{
		return m_playHandles;
	}

	void removePlayHandlesOfTypes(Track * track, const std::set<PlayHandleType>& types);


	// methods providing information for other classes
	inline fpp_t framesPerPeriod() const override
	{
		return m_framesPerPeriod;
	}


	AudioEngineProfiler& profiler()
	{
		return m_profiler;
	}

	void setProfilerOutputFile(QString path) override {
		m_profiler.setOutputFile(path);
	}

	int cpuLoad() const override
	{
		return m_profiler.cpuLoad();
	}

	const IAudioEngine::qualitySettings& currentQualitySettings() const override
	{
		return m_qualitySettings;
	}


	sample_rate_t baseSampleRate() const override;
	sample_rate_t outputSampleRate() const;
	sample_rate_t inputSampleRate() const;
	sample_rate_t processingSampleRate() const override;


	inline float masterGain() const override
	{
		return m_masterGain;
	}

	inline void setMasterGain(const float mo) override
	{
		m_masterGain = mo;
	}


	StereoSample getPeakValues(sampleFrame * ab, const f_cnt_t _frames) const override;


	bool criticalXRuns() const;

	inline bool hasFifoWriter() const
	{
		return m_fifoWriter != nullptr;
	}

	void pushInputFrames( sampleFrame * _ab, const f_cnt_t _frames );

	inline const sampleFrame * inputBuffer()
	{
		return m_inputBuffer[ m_inputBufferRead ];
	}

	inline f_cnt_t inputBufferFrames() const
	{
		return m_inputBufferFrames[ m_inputBufferRead ];
	}

	inline const surroundSampleFrame * nextBuffer()
	{
		return hasFifoWriter() ? m_fifo->read() : renderNextBuffer();
	}

	void changeQuality(const struct IAudioEngine::qualitySettings & qs) override;

	inline bool isMetronomeActive() const override { return m_metronomeActive; }
	inline void setMetronomeActive(bool value = true) override { m_metronomeActive = value; }

	//! Block until a change in model can be done (i.e. wait for audio thread)
	void requestChangeInModel() override;
	void doneChangeInModel() override;

	RequestChangesGuard requestChangesGuard()
	{
		return RequestChangesGuard{this};
	}

	bool isAudioDevNameValid(QString name) override;

	bool isMidiDevNameValid(QString name) override;

signals:
	void qualitySettingsChanged();


private:
	using Fifo = FifoBuffer<surroundSampleFrame*>;

	class fifoWriter : public QThread
	{
	public:
		fifoWriter( AudioEngine * audioEngine, Fifo * fifo );

		void finish();


	private:
		AudioEngine * m_audioEngine;
		Fifo * m_fifo;
		volatile bool m_writing;

		void run() override;

		void write( surroundSampleFrame * buffer );
	} ;


	AudioEngine( bool renderOnly );
	~AudioEngine() override;

	void startProcessing(bool needsFifo = true);
	void stopProcessing();


	AudioDevice * tryAudioDevices();
	MidiClient * tryMidiClients();


	const surroundSampleFrame * renderNextBuffer();

	void swapBuffers();

	void handleMetronome();

	void clearInternal();

	//! Called by the audio thread to give control to other threads,
	//! such that they can do changes in the model (like e.g. removing effects)
	void runChangesInModel();

	bool m_renderOnly;

	QVector<AudioPort *> m_audioPorts;

	fpp_t m_framesPerPeriod;

	sampleFrame * m_inputBuffer[2];
	f_cnt_t m_inputBufferFrames[2];
	f_cnt_t m_inputBufferSize[2];
	int m_inputBufferRead;
	int m_inputBufferWrite;

	surroundSampleFrame * m_outputBufferRead;
	surroundSampleFrame * m_outputBufferWrite;

	// worker thread stuff
	QVector<AudioEngineWorkerThread *> m_workers;
	int m_numWorkers;

	// playhandle stuff
	PlayHandleList m_playHandles;
	// place where new playhandles are added temporarily
	LocklessList<PlayHandle *> m_newPlayHandles;
	ConstPlayHandleList m_playHandlesToRemove;


	struct IAudioEngine::qualitySettings m_qualitySettings;
	float m_masterGain;

	bool m_isProcessing;

	// audio device stuff
	void doSetAudioDevice( AudioDevice *_dev );
	AudioDevice * m_audioDev;
	AudioDevice * m_oldAudioDev;
	QString m_audioDevName;
	bool m_audioDevStartFailed;

	// MIDI device stuff
	MidiClient * m_midiClient;
	QString m_midiClientName;

	// FIFO stuff
	Fifo * m_fifo;
	fifoWriter * m_fifoWriter;

	AudioEngineProfiler m_profiler;

	bool m_metronomeActive;

	bool m_clearSignal;

	bool m_changesSignal;
	unsigned int m_changes;
	QMutex m_changesMutex;
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
	QRecursiveMutex m_doChangesMutex;
#else
	QMutex m_doChangesMutex;
#endif
	QMutex m_waitChangesMutex;
	QWaitCondition m_changesAudioEngineCondition;
	QWaitCondition m_changesRequestCondition;

	bool m_waitingForWrite;

	friend class Engine;
	friend class AudioEngineWorkerThread;
	friend class ProjectRenderer;
} ;

} // namespace lmms

#endif // LMMS_AUDIO_ENGINE_H