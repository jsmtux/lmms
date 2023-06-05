/*
 * Effect.cpp - base-class for effects
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

#include <QDomElement>


#include "AudioEngine.h"
#include "AutomatableModel.h"
#include "Effect.h"
#include "EffectChain.h"
#include "EffectControls.h"
#include "TempoSyncKnobModel.h"
#include "ICoreApplication.h"

#include "ConfigManager.h"

namespace lmms
{


Effect::Effect( const Plugin::Descriptor * _desc,
			Model * _parent,
			const Descriptor::SubPluginFeatures::Key * _key ) :
	Plugin( _desc, _parent, _key ),
	m_parent( nullptr ),
	m_processors( 1 ),
	m_okay( true ),
	m_noRun( false ),
	m_running( false ),
	m_bufferCount( 0 ),
	m_enabledModel( new BoolModel( true, this, tr( "Effect enabled" ) ) ),
	m_wetDryModel( new FloatModel( 1.0f, -1.0f, 1.0f, 0.01f, this, tr( "Wet/Dry mix" ) ) ),
	m_gateModel( new FloatModel( 0.0f, 0.0f, 1.0f, 0.01f, this, tr( "Gate" ) ) ),
	m_autoQuitModel( new TempoSyncKnobModel( 1.0f, 1.0f, 8000.0f, 100.0f, 1.0f, this, tr( "Decay" ) ) ),
	m_autoQuitDisabled( false )
{
	m_srcState[0] = m_srcState[1] = nullptr;
	reinitSRC();
	
	if( ConfigManager::inst()->value( "ui", "disableautoquit").toInt() )
	{
		m_autoQuitDisabled = true;
	}
}




Effect::~Effect()
{
	for (const auto& state : m_srcState)
	{
		if (state != nullptr)
		{
			src_delete(state);
		}
	}
}

f_cnt_t Effect::timeout() const
{
	const float samples = getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->processingSampleRate() * m_autoQuitModel->value() / 1000.0f;
	return 1 + ( static_cast<int>( samples ) / getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->framesPerPeriod() );
}

void Effect::sampleDown( const sampleFrame * _src_buf,
						sampleFrame * _dst_buf,
						sample_rate_t _dst_sr )
{
	resample( 0, _src_buf,
			getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->processingSampleRate(),
				_dst_buf, _dst_sr,
				getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->framesPerPeriod() );
}

void Effect::sampleBack( const sampleFrame * _src_buf,
						sampleFrame * _dst_buf,
						sample_rate_t _src_sr )
{
	resample( 1, _src_buf, _src_sr, _dst_buf,
			getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->processingSampleRate(),
		getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->framesPerPeriod() * _src_sr /
			getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->processingSampleRate() );
}

void Effect::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	m_enabledModel->saveSettings( _doc, _this, "on" );
	m_wetDryModel->saveSettings( _doc, _this, "wet" );
	m_autoQuitModel->saveSettings( _doc, _this, "autoquit" );
	m_gateModel->saveSettings( _doc, _this, "gate" );
	controls()->saveState( _doc, _this );
}




void Effect::loadSettings( const QDomElement & _this )
{
	m_enabledModel->loadSettings( _this, "on" );
	m_wetDryModel->loadSettings( _this, "wet" );
	m_autoQuitModel->loadSettings( _this, "autoquit" );
	m_gateModel->loadSettings( _this, "gate" );

	QDomNode node = _this.firstChild();
	while( !node.isNull() )
	{
		if( node.isElement() )
		{
			if( controls()->nodeName() == node.nodeName() )
			{
				controls()->restoreState( node.toElement() );
			}
		}
		node = node.nextSibling();
	}
}





Effect * Effect::instantiate( const QString& pluginName,
				Model * _parent,
				Descriptor::SubPluginFeatures::Key * _key )
{
	Plugin * p = Plugin::instantiateWithKey( pluginName, _parent, _key );
	// check whether instantiated plugin is an effect
	if( dynamic_cast<Effect *>( p ) != nullptr )
	{
		// everything ok, so return pointer
		auto effect = dynamic_cast<Effect*>(p);
		effect->m_parent = dynamic_cast<EffectChain *>(_parent);
		return effect;
	}

	// not quite... so delete plugin and leave it up to the caller to instantiate a DummyEffect
	delete p;

	return nullptr;
}




void Effect::checkGate( double _out_sum )
{
	if( m_autoQuitDisabled )
	{
		return;
	}

	// Check whether we need to continue processing input.  Restart the
	// counter if the threshold has been exceeded.
	if( _out_sum - gate() <= typeInfo<float>::minEps() )
	{
		incrementBufferCount();
		if( bufferCount() > timeout() )
		{
			stopRunning();
			resetBufferCount();
		}
	}
	else
	{
		resetBufferCount();
	}
}


void Effect::reinitSRC()
{
	for (auto& state : m_srcState)
	{
		if (state != nullptr)
		{
			src_delete(state);
		}
		int error;
		const int currentInterpolation = getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->currentQualitySettings().libsrcInterpolation();
		if((state = src_new(currentInterpolation, DEFAULT_CHANNELS, &error)) == nullptr)
		{
			qFatal( "Error: src_new() failed in effect.cpp!\n" );
		}
	}
}




void Effect::resample( int _i, const sampleFrame * _src_buf,
							sample_rate_t _src_sr,
				sampleFrame * _dst_buf, sample_rate_t _dst_sr,
								f_cnt_t _frames )
{
	if( m_srcState[_i] == nullptr )
	{
		return;
	}
	m_srcData[_i].input_frames = _frames;
	m_srcData[_i].output_frames = getCoreApplication()->getEngineInteface()->getAudioEngineInterface()->framesPerPeriod();
	m_srcData[_i].data_in = const_cast<float*>(_src_buf[0].data());
	m_srcData[_i].data_out = _dst_buf[0].data ();
	m_srcData[_i].src_ratio = (double) _dst_sr / _src_sr;
	m_srcData[_i].end_of_input = 0;
	int error;
	if( ( error = src_process( m_srcState[_i], &m_srcData[_i] ) ) )
	{
		qFatal( "Effect::resample(): error while resampling: %s\n",
							src_strerror( error ) );
	}
}

} // namespace lmms
