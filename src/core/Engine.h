/*
 * Engine.h - engine-system of LMMS
 *
 * Copyright (c) 2006-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_ENGINE_H
#define LMMS_ENGINE_H

#include <QString>
#include <QObject>

#include "lmmsconfig.h"
#include "lmms_export.h"
#include "lmms_basics.h"
#include "TrackContainer.h"
#include "PatternStore.h"
#include "Mixer.h"

#include "IEngine.h"

namespace lmms
{

class AudioEngine;
class Mixer;
class ProjectJournal;
class Song;
class Ladspa2LMMS;
class AutomationTrack;
class IOscillator;

namespace gui
{
class GuiApplication;
}


class LMMS_EXPORT Engine : public IEngine
{
	Q_OBJECT
public:
	static void init( bool renderOnly );
	static void destroy();

	// core
	static AudioEngine *audioEngine()
	{
		return s_audioEngine;
	}

	static Mixer * mixer()
	{
		return s_mixer;
	}

	IMixer* getMixerInterface() override {
		return s_mixer;
	}

	static Song * getSong()
	{
		return s_song;
	}

	static PatternStore * patternStore()
	{
		return s_patternStore;
	}

    IPatternStore* getPatternStoreInterface() override {
		return s_patternStore;
	}

	static ProjectJournal * projectJournal()
	{
		return s_projectJournal;
	}

	static bool ignorePluginBlacklist();

#ifdef LMMS_HAVE_LV2
	static class Lv2Manager * getLv2Manager()
	{
		return s_lv2Manager;
	}
#endif

	static Ladspa2LMMS * getLADSPAManager()
	{
		return s_ladspaManager;
	}

	float getFramesPerTick() override {
		return Engine::framesPerTick();
	}

	static float framesPerTick()
	{
		return s_framesPerTick;
	}

	static float framesPerTick(sample_rate_t sample_rate);

	static void updateFramesPerTick();

	static void setDndPluginKey(PluginDescriptor::Key* newKey);
	PluginDescriptor::Key* pickDndPluginKey() override;

	static TrackList getTracks(bool includeGlobalAutomation = false);
	TrackList getITracks(bool includeGlobalAutomation = false) override {
		return Engine::getTracks(includeGlobalAutomation);
	}
	static AutomationTrack* getGlobalAutomationTrack();
    ISong* getSongInterface() override;
    IAudioEngine* getAudioEngineInterface() override;
	IProjectJournal* getProjectJournalInterface() override;

	IDetuningHelper* createDetuningHelper() override;


    IAutomatableModel<float>* createFloatModel(float val, float min, float max, float step,
        QObject * parent,
        const QString& displayName) override
	{
		return new FloatModel(val, min, max, step, parent, displayName);
	}
    IAutomatableModel<int>* createIntModel(int val, int min, int max, int,
        QObject * parent,
        const QString& displayName) override
	{
		return new IntModel(val, min, max, parent, displayName);
	}
    IAutomatableModel<bool>* createBoolModel(bool val, bool, bool, bool,
        QObject * parent,
        const QString& displayName) override
	{
		return new BoolModel(val, parent, displayName);
	}
	IComboBoxModelWrapper* createComboBox(QObject* parent,
				const QString& displayName) override
	{
		return new ComboBoxModelWrapper(parent, displayName);
	}

	std::unique_ptr<ISampleBuffer> createSampleBuffer() override;
	IHandleState* createHandleState(bool varyingPitch, int interpolationMode) override;
	
    IOscillator* createOscillator(const IIntAutomatableModel *wave_shape_model,
		const IIntAutomatableModel *mod_algo_model,
		const float &freq,
		const float &detuning_div_samplerate,
		const float &phase_offset,
		const float &volume,
		IOscillator *m_subOsc = nullptr) override;
signals:
	void initProgress(const QString &msg);


private:
	// small helper function which sets the pointer to NULL before actually deleting
	// the object it refers to
	template<class T>
	static inline void deleteHelper(T** ptr)
	{
		T* tmp = *ptr;
		*ptr = nullptr;
		delete tmp;
	}

	static float s_framesPerTick;

	// core
	static AudioEngine *s_audioEngine;
	static Mixer * s_mixer;
	static Song * s_song;
	static PatternStore * s_patternStore;
	static ProjectJournal * s_projectJournal;

#ifdef LMMS_HAVE_LV2
	static class Lv2Manager* s_lv2Manager;
#endif
	static Ladspa2LMMS* s_ladspaManager;
	static PluginDescriptor::Key* s_dndPluginKey;

	// even though most methods are static, an instance is needed for Qt slots/signals
	static Engine* s_instanceOfMe;

	friend class gui::GuiApplication;
};

} // namespace lmms

#endif // LMMS_ENGINE_H
