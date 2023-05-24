/*
 * EffectChain.h - class for processing and effects chain
 *
 * Copyright (c) 2006-2008 Danny McRae <khjklujn/at/users.sourceforge.net>
 * Copyright (c) 2008-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_EFFECT_CHAIN_H
#define LMMS_EFFECT_CHAIN_H

#include "IEffectChain.h"

#include "Model.h"
#include "AutomatableModel.h"

namespace lmms
{

class IEffect;

namespace gui
{

class EffectRackView;

} // namespace gui


class LMMS_EXPORT EffectChain : public IEffectChain
{
	Q_OBJECT
public:
	EffectChain( QObject * _parent );
	~EffectChain() override;

	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	inline QString nodeName() const override
	{
		return "fxchain";
	}

	void appendEffect( IEffect * _effect ) override;
	void removeEffect( IEffect * _effect ) override;
	void moveDown( IEffect * _effect );
	void moveUp( IEffect * _effect ) override;
	bool processAudioBuffer( sampleFrame * _buf, const fpp_t _frames, bool hasInputNoise );
	void startRunning();
	IBoolAutomatableModel* enabledModel() override {return &m_enabledModel;}

	void clear();

	Model* model() override { return &m_model; }
	EffectList& effects() override {return m_effects;}
private:
	Model m_model;
	EffectList m_effects;

	BoolModel m_enabledModel;

signals:
	void aboutToClear();

} ;

} // namespace lmms

#endif // LMMS_EFFECT_CHAIN_H
