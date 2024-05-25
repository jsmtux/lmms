/*
 * AudioAAUdio.h - device-class that performs PCM-output via AAudio
 *
 * Copyright (c) 2024 JM Ventoso Picos <jsmtux/at/gmail.com>
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

#ifndef LMMS_AUDIO_AAUDIO_H
#define LMMS_AUDIO_AAUDIO_H

#include "lmmsconfig.h"

#ifdef LMMS_HAVE_AAUDIO

#include <cmath>
#include <aaudio/AAudio.h>
#include "AudioDevice.h"
#include "AudioEngine.h"
#include <QDebug>

namespace lmms
{

class AudioAAudio : public AudioDevice
{
public:
  AudioAAudio(bool & _success_ful, AudioEngine* _audioEngine)
      : AudioDevice(DEFAULT_CHANNELS, _audioEngine)
  {
    // auto devices = AudioManager::getDevices(AudioManager::GET_DEVICES_OUTPUTS);

    AAudioStreamBuilder *builder;
    aaudio_result_t result = AAudio_createStreamBuilder(&builder);
    assert(result == AAUDIO_OK);

    // AAudioStreamBuilder_setDeviceId(builder, devices[0].deviceId());
    AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_EXCLUSIVE);
    AAudioStreamBuilder_setSampleRate(builder, sampleRate());
    AAudioStreamBuilder_setChannelCount(builder, DEFAULT_CHANNELS);
    AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_FLOAT);
    // This will be filled dynamically by testing the device
    // https://developer.android.com/ndk/guides/audio/aaudio/aaudio#tuning-buffers
    // AAudioStreamBuilder_setBufferCapacityInFrames(builder, frames);

    m_buffer = new sampleFrame[audioEngine()->framesPerPeriod()];

    AAudioStreamBuilder_setDataCallback(builder, AudioAAudio::aAudioDataCallback, this);
    result = AAudioStreamBuilder_openStream(builder, &m_stream);
    assert(result == AAUDIO_OK);
    _success_ful = true;
  }

  ~AudioAAudio() {
	  stopProcessing();
    delete[] m_buffer;
  }

	inline static QString name()
	{
		return "AAudio";
	}


private:
  void startProcessing() override {
    AAudioStream_requestStart(m_stream);
  }

  void stopProcessing() override {
    AAudioStream_close(m_stream);
  }

  void applyQualitySettings() override {

  }

  static aaudio_data_callback_result_t aAudioDataCallback(
      AAudioStream *stream,
      void *userData,
      void *audioData,
      int32_t numFrames) {
    auto* object = static_cast<AudioAAudio*>(userData);
    return object->aAudioDataCallback(stream, static_cast<float*>(audioData), numFrames);
  }

  aaudio_data_callback_result_t aAudioDataCallback(
      AAudioStream *stream,
      float *audioData,
      int32_t audioDataSize) {
    auto remainingDataFrames = audioDataSize - 1;
    qDebug() << "Remaning frames is " << remainingDataFrames << Qt::endl;
    while(remainingDataFrames > 0) {
      if (m_buffer_frames_processed >= m_buffer_frames_size) {
        m_buffer_frames_size = getNextBuffer(m_buffer);
        m_buffer_frames_processed = 0;
        qDebug() << "Fetched " << m_buffer_frames_size << " frames\n";
        if (m_buffer_frames_size == 0) {
          memset( audioData, 0, remainingDataFrames * sizeof(sampleFrame));
          break;
        }
        // for(int i = 0; i < m_buffer_frames_size; i++) {
        //   for(auto& sample : m_buffer[i]) {
        //     sample *= audioEngine()->masterGain();
        //   }
        // }
      }

      auto to_copy = qMin(remainingDataFrames, m_buffer_frames_size - m_buffer_frames_processed);

      memcpy( audioData, m_buffer + m_buffer_frames_processed, to_copy * sizeof(sampleFrame) );

      remainingDataFrames -= to_copy;
      audioData += to_copy * 2;
      m_buffer_frames_processed += to_copy;
    }
    qDebug() << "Returning]n\n";
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
  }

  sampleFrame* m_buffer;
  fpp_t m_buffer_frames_size{0};
  fpp_t m_buffer_frames_processed{0};
  AAudioStream *m_stream;
};

}

#endif // LMMS_HAVE_AAUDIO
#endif // LMMS_AUDIO_AAUDIO_H
