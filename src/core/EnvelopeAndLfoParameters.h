/*
 * EnvelopeAndLfoParameters.h - class EnvelopeAndLfoParameters
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_ENVELOPE_AND_LFO_PARAMETERS_H
#define LMMS_ENVELOPE_AND_LFO_PARAMETERS_H

#include <QVector>

#include "IEnvelopeAndLfoParameters.h"

#include "JournallingObject.h"
#include "AutomatableModel.h"
#include "SampleBuffer.h"
#include "TempoSyncKnobModel.h"
#include "lmms_basics.h"

namespace lmms
{

class LMMS_EXPORT EnvelopeAndLfoParameters : public IEnvelopeAndLfoParameters, public JournallingObject
{
	Q_OBJECT
public:
	class LfoInstances
	{
	public:
		LfoInstances() = default;

		~LfoInstances() = default;

		inline bool isEmpty() const
		{
			return m_lfos.isEmpty();
		}

		void trigger();
		void reset();

		void add( EnvelopeAndLfoParameters * lfo );
		void remove( EnvelopeAndLfoParameters * lfo );

	private:
		QMutex m_lfoListMutex;
		using LfoList = QList<EnvelopeAndLfoParameters*>;
		LfoList m_lfos;

	} ;

	EnvelopeAndLfoParameters( float _value_for_zero_amount,
							QObject * _parent );
	~EnvelopeAndLfoParameters() override;

	static inline float expKnobVal( float _val )
	{
		return ( ( _val < 0 ) ? -_val : _val ) * _val;
	}

	static LfoInstances * instances()
	{
		return s_lfoInstances;
	}

	void fillLevel( float * _buf, f_cnt_t _frame,
				const f_cnt_t _release_begin,
				const fpp_t _frames );

	inline bool isUsed() const
	{
		return m_used;
	}


	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;
	QString nodeName() const override
	{
		return "el";
	}

	inline f_cnt_t PAHD_Frames() const
	{
		return m_pahdFrames;
	}

	inline f_cnt_t releaseFrames() const
	{
		return m_rFrames;
	}

	IFloatAutomatableModel* predelayModel() override
	{
		return &m_predelayModel;
	}

	IFloatAutomatableModel* attackModel() override
	{
		return &m_attackModel;
	}

	IFloatAutomatableModel* holdModel() override
	{
		return &m_holdModel;
	}

	IFloatAutomatableModel* decayModel() override
	{
		return &m_decayModel;
	}

	IFloatAutomatableModel* sustainModel() override
	{
		return &m_sustainModel;
	}

	IFloatAutomatableModel* releaseModel() override
	{
		return &m_releaseModel;
	}

	IFloatAutomatableModel* amountModel() override
	{
		return &m_amountModel;
	}

	IFloatAutomatableModel* lfoPredelayModel() override
	{
		return &m_lfoPredelayModel;
	}

	IFloatAutomatableModel* lfoAttackModel() override
	{
		return &m_lfoAttackModel;
	}

	ITempoSyncKnobModelWrapper* lfoSpeedModel() override
	{
		return &m_lfoSpeedModel;
	}

	IFloatAutomatableModel* lfoAmountModel() override
	{
		return &m_lfoAmountModel;
	}

	IIntAutomatableModel* lfoWaveModel() override
	{
		return &m_lfoWaveModel;
	}
	IBoolAutomatableModel* x100Model() override
	{
		return &m_x100Model;
	}
	IBoolAutomatableModel* controlEnvAmountModel() override
	{
		return &m_controlEnvAmountModel;
	}
	ISampleBuffer* userWave() override
	{
		return &m_userWave;
	}
	f_cnt_t lfoPredelayFrames() override
	{
		return m_lfoPredelayFrames;
	}
	f_cnt_t lfoOscillationFrames() override
	{
		return m_lfoOscillationFrames;
	}
	f_cnt_t lfoAttackFrames() override
	{
		return m_lfoAttackFrames;
	}


public slots:
	void updateSampleVars();


protected:
	void fillLfoLevel( float * _buf, f_cnt_t _frame, const fpp_t _frames );


private:
	static LfoInstances * s_lfoInstances;
	bool m_used;

	QMutex m_paramMutex;

	FloatModel m_predelayModel;
	FloatModel m_attackModel;
	FloatModel m_holdModel;
	FloatModel m_decayModel;
	FloatModel m_sustainModel;
	FloatModel m_releaseModel;
	FloatModel m_amountModel;

	float  m_sustainLevel;
	float  m_amount;
	float  m_valueForZeroAmount;
	float  m_amountAdd;
	f_cnt_t m_pahdFrames;
	f_cnt_t m_rFrames;
	sample_t * m_pahdEnv;
	sample_t * m_rEnv;
	f_cnt_t m_pahdBufSize;
	f_cnt_t m_rBufSize;


	FloatModel m_lfoPredelayModel;
	FloatModel m_lfoAttackModel;
	TempoSyncKnobModelWrapper m_lfoSpeedModel;
	FloatModel m_lfoAmountModel;
	IntModel m_lfoWaveModel;

	BoolModel m_x100Model;
	BoolModel m_controlEnvAmountModel;


	f_cnt_t m_lfoPredelayFrames;
	f_cnt_t m_lfoAttackFrames;
	f_cnt_t m_lfoOscillationFrames;
	f_cnt_t m_lfoFrame;
	float m_lfoAmount;
	bool m_lfoAmountIsZero;
	sample_t * m_lfoShapeData;
	sample_t m_random;
	bool m_bad_lfoShapeData;
	SampleBuffer m_userWave;

	sample_t lfoShapeSample( fpp_t _frame_offset );
	void updateLfoShapeData();
} ;

} // namespace lmms

#endif // LMMS_ENVELOPE_AND_LFO_PARAMETERS_H
