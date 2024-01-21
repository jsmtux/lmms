/*
 * peak_controller_Effect.h - PeakController effect plugin
 *
 * Copyright (c) 2008 Paul Giblock <drfaygo/at/gmail/dot/com>
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


#ifndef PEAK_CONTROLLER_EFFECT_H
#define PEAK_CONTROLLER_EFFECT_H

#include "Effect.h"
#include "IController.h"
#include "PeakControllerEffectControls.h"

namespace lmms
{

class PeakControllerEffect : public Effect, public IPeakControllerEffect
{
public:
	PeakControllerEffect( Model * parent, 
						const PluginDescriptor::Key * _key );
	~PeakControllerEffect() override;
	bool processAudioBuffer( sampleFrame * _buf,
									const fpp_t _frames ) override;

	void removeFromEffectChain() override {
		if( m_peakEffect != nullptr && m_peakEffect->effectChain() != nullptr )
		{
			m_peakEffect->effectChain()->removeEffect( m_peakEffect );
		}
	}

	EffectControls * controls() override
	{
		return &m_peakControls;
	}

	float lastSample() override;
	{
		return m_lastSample;
	}

	int effectId() override {
		return m_effectId;
	}

    void setEffectId(int effect_id) {
		m_effectId = effect_id;
	}

	IPeakController * controller()
	{
		return m_autoController;
	}
	
	FloatModel * attackModel() override
	{
		return &( m_peakControls.m_attackModel );
	}

	FloatModel * decayModel() override
	{
		return &( m_peakControls.m_decayModel );
	}

	int m_effectId;

private:
	PeakControllerEffectControls m_peakControls;

	float m_lastSample;

	PeakController * m_autoController;

	friend class PeakControllerEffectControls;

} ;


} // namespace lmms

#endif
