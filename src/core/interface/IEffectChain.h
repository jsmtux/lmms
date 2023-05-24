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

#ifndef LMMS_I_EFFECT_CHAIN_H
#define LMMS_I_EFFECT_CHAIN_H

#include "IModels.h"
#include "SerializingObject.h"

namespace lmms
{

class IEffect;

namespace gui
{

class EffectRackView;

} // namespace gui


class LMMS_EXPORT IEffectChain : public QObject, public SerializingObject
{
	Q_OBJECT
public:
	using EffectList = QVector<IEffect*>;

	virtual Model* model() = 0;
    virtual void removeEffect( IEffect * _effect ) = 0;
	virtual IBoolAutomatableModel* enabledModel() = 0;
	virtual void appendEffect( IEffect * _effect ) = 0;
	virtual void moveUp( IEffect * _effect ) = 0;
	virtual EffectList& effects() = 0;
} ;

} // namespace lmms

#endif // LMMS_I_EFFECT_CHAIN_H
