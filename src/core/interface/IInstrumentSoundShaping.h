/*
 * InstrumentSoundShaping.h - declaration of class InstrumentSoundShaping
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_I_INSTRUMENT_SOUND_SHAPING_H
#define LMMS_I_INSTRUMENT_SOUND_SHAPING_H

#include "IModels.h"
#include "IEnvelopeAndLfoParameters.h"

namespace lmms
{

class IInstrumentSoundShaping
{
public:
	enum Targets
	{
		Volume,
		Cut,
		Resonance,
		NumTargets
	} ;
	virtual IEnvelopeAndLfoParameters * envLfoParameters(int targetIndex) = 0;
	virtual const char* targetName(int targetIndex) const = 0;
	virtual IBoolAutomatableModel* filterEnabledModel() = 0;
	virtual IComboBoxModelWrapper* filterModel() = 0;
	virtual IFloatAutomatableModel* filterCutModel() = 0;
	virtual IFloatAutomatableModel* filterResModel() = 0;
	virtual Model* model() = 0;
} ;


} // namespace lmms

#endif // LMMS_I_INSTRUMENT_SOUND_SHAPING_H
