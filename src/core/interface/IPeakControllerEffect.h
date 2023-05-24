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

#ifndef I_PEAK_CONTROLLER_EFFECT_H
#define I_PEAK_CONTROLLER_EFFECT_H

#include "IModels.h"
#include "IController.h"
#include "IEffectChain.h"

namespace lmms {

class IPeakControllerEffect {
public:
    virtual IEffectChain* effectChain() const = 0;
	virtual Model* model();
	virtual IFloatAutomatableModel * attackModel() = 0;
	virtual IFloatAutomatableModel * decayModel() = 0;
    virtual int effectId() = 0;
    virtual void setEffectId(int effect_id) = 0;
    virtual float lastSample() = 0;
    virtual void removeFromEffectChain() = 0;
    virtual IPeakController * controller() = 0;
};

}

#endif // I_PEAK_CONTROLLER_EFFECT_H