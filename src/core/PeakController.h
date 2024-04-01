/*
 * PeakController.h - peak-controller class
 *
 * Copyright (c) 2008-2009 Paul Giblock <drfaygo/at/gmail.com>
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

#ifndef LMMS_PEAK_CONTROLLER_H
#define LMMS_PEAK_CONTROLLER_H

#include "Controller.h"
#include "IPeakControllerEffect.h"

namespace lmms
{


class IPeakControllerEffect;

using PeakControllerEffectVector = QVector<IPeakControllerEffect*>;

class LMMS_EXPORT PeakController : public Controller, public IPeakController
{
	Q_OBJECT
public:
	PeakController( QObject * _parent,
		IPeakControllerEffect *_peak_effect = nullptr );


	~PeakController() override;

	void saveSettings( QDomDocument & _doc, QDomElement & _this ) override;
	void loadSettings( const QDomElement & _this ) override;
	QString nodeName() const override;

	static void initGetControllerBySetting();
	static PeakController * getControllerBySetting( const QDomElement & _this );

	static PeakControllerEffectVector s_effects;


public slots:
	void handleDestroyedEffect();
	void updateCoeffs();

protected:
	// The internal per-controller get-value function
	void updateValueBuffer() override;

	IPeakControllerEffect * m_peakEffect;

	friend class PeakControllerDialog;

private:
	float m_currentSample;
	//backward compatibility for <= 0.4.15
	static int m_getCount;
	static int m_loadCount;
	static bool m_buggedFile;
	
	float m_attackCoeff;
	float m_decayCoeff;
	bool m_coeffNeedsUpdate;
} ;

} // namespace lmms

#endif // LMMS_PEAK_CONTROLLER_H