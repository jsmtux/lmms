/*
 * Effect.h - base class for effects
 *
 * Copyright (c) 2006-2007 Danny McRae <khjklujn/at/users.sourceforge.net>
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

#ifndef LMMS_EFFECT_H
#define LMMS_EFFECT_H

#include "Plugin.h"
#include "IMemoryManager.h"
#include "IModels.h"
#include "samplerate.h"
#include "IEffectChain.h"

#include <QObject>

namespace lmms
{

namespace gui
{

class EffectView;

} // namespace gui


class LMMS_EXPORT Effect : public IEffect, public Plugin
{
	I_MM_OPERATORS
	Q_OBJECT
public:
	Effect( const PluginDescriptor * _desc,
			IEffectChain * _parent,
			const PluginDescriptor::Key * _key );
	~Effect() override;

	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	IEffectChain* model() override {
		return m_effectChain;
	}

	inline QString nodeName() const override
	{
		return "effect";
	}

	
	virtual bool processAudioBuffer( sampleFrame * _buf,
						const fpp_t _frames ) override = 0;

	inline ch_cnt_t processorCount() const
	{
		return m_processors;
	}

	inline void setProcessorCount( ch_cnt_t _processors )
	{
		m_processors = _processors;
	}

	inline bool isOkay() const override
	{
		return m_okay;
	}

	inline void setOkay( bool _state )
	{
		m_okay = _state;
	}


	inline bool isRunning() const override
	{
		return m_running;
	}

	inline void startRunning() override
	{ 
		m_bufferCount = 0;
		m_running = true; 
	}

	inline void stopRunning()
	{
		m_running = false;
	}

	inline bool isEnabled() const
	{
		return m_enabledModel->value();
	}

	f_cnt_t timeout() const;

	inline float wetLevel() const
	{
		return m_wetDryModel->value();
	}

	inline float dryLevel() const
	{
		return 1.0f - m_wetDryModel->value();
	}

	inline float gate() const
	{
		const float level = m_gateModel->value();
		return level*level * m_processors;
	}

	inline f_cnt_t bufferCount() const
	{
		return m_bufferCount;
	}

	inline void resetBufferCount()
	{
		m_bufferCount = 0;
	}

	inline void incrementBufferCount()
	{
		++m_bufferCount;
	}

	inline bool dontRun() const
	{
		return m_noRun;
	}

	inline void setDontRun( bool _state )
	{
		m_noRun = _state;
	}

	IEffectChain * effectChain() const
	{
		return m_parent;
	}

	virtual bool isDummy() override
	{
		return false;
	}

	static Effect * instantiate( const QString & _plugin_name,
				IEffectChain * _parent,
				PluginDescriptor::Key * _key );

	IAutomatableModel<bool>* enabledModel() override {
		return m_enabledModel.get();
	};
	IAutomatableModel<float>* wetDryModel() override {
		return m_wetDryModel.get();
	};
	IAutomatableModel<float>* gateModel() override {
		return m_gateModel.get();
	};
	ITempoSyncKnobModelWrapper* autoQuitModel() override {
		return m_autoQuitModel.get();
	};
	virtual QString displayName() override {
		return m_parent->model()->displayName();
	}

	bool autoQuitDisabled() override {
		return m_autoQuitDisabled;
	}

protected:
	/**
		Effects should call this at the end of audio processing

		If the setting "Keep effects running even without input" is disabled,
		after "decay" ms of a signal below "gate", the effect is turned off
		and won't be processed again until it receives new audio input
	*/
	void checkGate( double _out_sum );


	// some effects might not be capable of higher sample-rates so they can
	// sample it down before processing and back after processing
	void sampleDown( const sampleFrame * _src_buf,
							sampleFrame * _dst_buf,
							sample_rate_t _dst_sr );

	void sampleBack( const sampleFrame * _src_buf,
							sampleFrame * _dst_buf,
							sample_rate_t _src_sr );

	void reinitSRC();


private:
	IEffectChain * m_parent;
	void resample( int _i, const sampleFrame * _src_buf,
					sample_rate_t _src_sr,
					sampleFrame * _dst_buf, sample_rate_t _dst_sr,
					const f_cnt_t _frames );

	IEffectChain* m_effectChain;

	ch_cnt_t m_processors;

	bool m_okay;
	bool m_noRun;
	bool m_running;
	f_cnt_t m_bufferCount;

	std::unique_ptr<IAutomatableModel<bool>> m_enabledModel;
	std::unique_ptr<IAutomatableModel<float>> m_wetDryModel;
	std::unique_ptr<IAutomatableModel<float>> m_gateModel;
	std::unique_ptr<ITempoSyncKnobModelWrapper> m_autoQuitModel;
	
	bool m_autoQuitDisabled;

	SRC_DATA m_srcData[2];
	SRC_STATE * m_srcState[2];

	friend class EffectChain;

} ;

} // namespace lmms

#endif // LMMS_EFFECT_H
