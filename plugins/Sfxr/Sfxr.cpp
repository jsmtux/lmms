/*
 * Sfxr.cpp - port of sfxr to LMMS
 * Originally written by Tomas Pettersson. For the original license,
 * please read readme.txt in this directory
 *
 * Copyright (c) 2014 Wong Cho Ching
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
 */

#include <cstdlib>
#include <ctime>

#define rnd(n) (rand()%(n+1))

#define PI 3.14159265f

float frnd(float range)
{
	return (float)rnd(10000)/10000*range;
}

#include <cmath>

#include <QDomElement>

#include "Sfxr.h"
#include "AudioEngine.h"
#include "Engine.h"
#include "InstrumentTrack.h"
#include "NotePlayHandle.h"
#include "MidiEvent.h"

#include "widgets/Knob.h"
#include "widgets/PixmapButton.h"

#include "embed.h"

#include "plugin_export.h"

namespace lmms
{


extern "C"
{

Plugin::Descriptor PLUGIN_EXPORT sfxr_plugin_descriptor =
{
	LMMS_STRINGIFY( PLUGIN_NAME ),
	"sfxr",
	QT_TRANSLATE_NOOP( "PluginBrowser",
				"LMMS port of sfxr" ),
	"Wong Cho Ching",
	0x0100,
	Plugin::Instrument,
	new PluginPixmapLoader( "logo" ),
	nullptr,
	nullptr,
} ;

}




SfxrSynth::SfxrSynth( const SfxrInstrument * s ):
	s(s),
	playing_sample( true )
{
    resetSample( false );
}




void SfxrSynth::resetSample( bool restart )
{
	if(!restart)
	{
		phase=0;
	}
	fperiod=100.0/(s->m_startFreqModel.value()*s->m_startFreqModel.value()+0.001);
	period=(int)fperiod;
	fmaxperiod=100.0/(s->m_minFreqModel.value()*s->m_minFreqModel.value()+0.001);
	fslide=1.0-pow((double)s->m_slideModel.value(), 3.0)*0.01;
	fdslide=-pow((double)s->m_dSlideModel.value(), 3.0)*0.000001;
	square_duty=0.5f-s->m_sqrDutyModel.value()*0.5f;
	square_slide=-s->m_sqrSweepModel.value()*0.00005f;
	if(s->m_changeAmtModel.value()>=0.0f)
		arp_mod=1.0-pow((double)s->m_changeAmtModel.value(), 2.0)*0.9;
	else
		arp_mod=1.0+pow((double)s->m_changeAmtModel.value(), 2.0)*10.0;
	arp_time=0;
	arp_limit=(int)(pow(1.0f-s->m_changeSpeedModel.value(), 2.0f)*20000+32);
	if(s->m_changeSpeedModel.value()==1.0f)
		arp_limit=0;
	if(!restart)
	{
		// reset filter
		fltp=0.0f;
		fltdp=0.0f;
		fltw=pow(s->m_lpFilCutModel.value(), 3.0f)*0.1f;
		fltw_d=1.0f+s->m_lpFilCutSweepModel.value()*0.0001f;
		fltdmp=5.0f/(1.0f+pow(s->m_lpFilResoModel.value(), 2.0f)*20.0f)*(0.01f+fltw);
		if(fltdmp>0.8f) fltdmp=0.8f;
		fltphp=0.0f;
		flthp=pow(s->m_hpFilCutModel.value(), 2.0f)*0.1f;
		flthp_d=1.0+s->m_hpFilCutSweepModel.value()*0.0003f;
		// reset vibrato
		vib_phase=0.0f;
		vib_speed=pow(s->m_vibSpeedModel.value(), 2.0f)*0.01f;
		vib_amp=s->m_vibDepthModel.value()*0.5f;
		// reset envelope
		env_vol=0.0f;
		env_stage=0;
		env_time=0;

		env_length[0]=(int)(s->m_attModel.value()*s->m_attModel.value()*99999.0f)+1;
		env_length[1]=(int)(s->m_holdModel.value()*s->m_holdModel.value()*99999.0f)+1;
		env_length[2]=(int)(s->m_decModel.value()*s->m_decModel.value()*99999.0f)+1;

		fphase=pow(s->m_phaserOffsetModel.value(), 2.0f)*1020.0f;
		if(s->m_phaserOffsetModel.value()<0.0f) fphase=-fphase;
		fdphase=pow(s->m_phaserSweepModel.value(), 2.0f)*1.0f;
		if(s->m_phaserSweepModel.value()<0.0f) fdphase=-fdphase;
		iphase=abs((int)fphase);
		ipp=0;
		
		phaser_buffer.fill(0.0f);
		for (auto& noiseSample : noise_buffer) 
		{
			noiseSample = frnd(2.0f) - 1.0f; 
		}

		rep_time=0;
		rep_limit=(int)(pow(1.0f-s->m_repeatSpeedModel.value(), 2.0f)*20000+32);
		if(s->m_repeatSpeedModel.value()==0.0f)
			rep_limit=0;
    }
}




void SfxrSynth::update( sampleFrame * buffer, const int32_t frameNum )
{
	for(int i=0;i<frameNum;i++)
	{
        if(!playing_sample)
        {
            for( ch_cnt_t j=0; j < DEFAULT_CHANNELS; j++ )
            {
                buffer[i][j]=0.0f;
            }
        }

		rep_time++;
		if(rep_limit!=0 && rep_time>=rep_limit)
		{
			rep_limit=0;
			resetSample(true);
		}

		// frequency envelopes/arpeggios
		arp_time++;
		if(arp_limit!=0 && arp_time>=arp_limit)
		{
			arp_limit=0;
			fperiod*=arp_mod;
		}
		fslide+=fdslide;
		fperiod*=fslide;
		if(fperiod>fmaxperiod)
		{
			fperiod=fmaxperiod;
			if(s->m_minFreqModel.value()>0.0f)
				playing_sample=false;
		}
		float rfperiod=fperiod;
		if(vib_amp>0.0f)
		{
			vib_phase+=vib_speed;
			rfperiod=fperiod*(1.0+sin(vib_phase)*vib_amp);
		}
		period=(int)rfperiod;
		if(period<8) period=8;
		square_duty+=square_slide;
		if(square_duty<0.0f) square_duty=0.0f;
		if(square_duty>0.5f) square_duty=0.5f;
		// volume envelope
		env_time++;
		if(env_time>env_length[env_stage])
		{
			env_time=0;
			env_stage++;
			if(env_stage==3)
				playing_sample=false;
		}
		if(env_stage==0)
			env_vol=(float)env_time/env_length[0];
		if(env_stage==1)
			env_vol=1.0f+pow(1.0f-(float)env_time/env_length[1], 1.0f)*2.0f*s->m_susModel.value();
		if(env_stage==2)
			env_vol=1.0f-(float)env_time/env_length[2];

		// phaser step
		fphase+=fdphase;
		iphase=abs((int)fphase);
		if(iphase>1023) iphase=1023;

		if(flthp_d!=0.0f)
		{
			flthp*=flthp_d;
			if(flthp<0.00001f) flthp=0.00001f;
			if(flthp>0.1f) flthp=0.1f;
		}

		float ssample=0.0f;
		for(int si=0;si<8;si++) // 8x supersampling
		{
			float sample=0.0f;
			phase++;
			if(phase>=period)
			{
//				phase=0;
				phase%=period;
				if(s->m_waveFormModel.value()==3)
					for (auto& noiseSample : noise_buffer) 
					{
						noiseSample = frnd(2.0f) - 1.0f;
					}
			}
			// base waveform
			float fp=(float)phase/period;
			switch(s->m_waveFormModel.value())
			{
			case 0: // square
				if(fp<square_duty)
					sample=0.5f;
				else
					sample=-0.5f;
				break;
			case 1: // sawtooth
				sample=1.0f-fp*2;
				break;
			case 2: // sine
				sample=(float)sin(fp*2*PI);
				break;
			case 3: // noise
				sample=noise_buffer[phase*32/period];
				break;
			}
			// lp filter
			float pp=fltp;
			fltw*=fltw_d;
			if(fltw<0.0f) fltw=0.0f;
			if(fltw>0.1f) fltw=0.1f;
			if(s->m_lpFilCutModel.value()!=1.0f)
			{
				fltdp+=(sample-fltp)*fltw;
				fltdp-=fltdp*fltdmp;
			}
			else
			{
				fltp=sample;
				fltdp=0.0f;
			}
			fltp+=fltdp;
			// hp filter
			fltphp+=fltp-pp;
			fltphp-=fltphp*flthp;
			sample=fltphp;
			// phaser
			phaser_buffer[ipp&1023]=sample;
			sample+=phaser_buffer[(ipp-iphase+1024)&1023];
			ipp=(ipp+1)&1023;
			// final accumulation and envelope application
			ssample+=sample*env_vol;
		}
		//ssample=ssample/8*master_vol;

		//ssample*=2.0f*sound_vol;
		ssample*=0.025f;

		if(buffer!=nullptr)
		{
			if(ssample>1.0f) ssample=1.0f;
			if(ssample<-1.0f) ssample=-1.0f;
			for( ch_cnt_t j=0; j<DEFAULT_CHANNELS; j++ )
			{
				buffer[i][j]=ssample;
			}
		}
	}
}




bool SfxrSynth::isPlaying() const
{
	return playing_sample;
}




SfxrInstrument::SfxrInstrument( InstrumentTrack * _instrument_track ) :
	QWidgetInstrumentPlugin( _instrument_track, &sfxr_plugin_descriptor ),
	m_attModel(0.0f, model(), "Attack Time"),
	m_holdModel(0.3f, model(), "Sustain Time"),
	m_susModel(0.0f, model(), "Sustain Punch"),
	m_decModel(0.4f, model(), "Decay Time"),

	m_startFreqModel(0.3f, model(), "Start Frequency"),
	m_minFreqModel(0.0f, model(), "Min Frequency"),
	m_slideModel(0.0f, model(), "Slide"),
	m_dSlideModel(0.0f, model(), "Delta Slide"),
	m_vibDepthModel(0.0f, model(), "Vibrato Depth"),
	m_vibSpeedModel(0.0f, model(), "Vibrato Speed"),

	m_changeAmtModel(0.0f, model(), "Change Amount"),
	m_changeSpeedModel(0.0f, model(), "Change Speed"),

	m_sqrDutyModel(0.0f, model(), "Square Duty"),
	m_sqrSweepModel(0.0f, model(), "Duty Sweep"),

	m_repeatSpeedModel(0.0f, model(), "Repeat Speed"),

	m_phaserOffsetModel(0.0f, model(), "Phaser Offset"),
	m_phaserSweepModel(0.0f, model(), "Phaser Sweep"),

	m_lpFilCutModel(1.0f, model(), "LP Filter Cutoff"),
	m_lpFilCutSweepModel(0.0f, model(), "LP Filter Cutoff Sweep"),
	m_lpFilResoModel(0.0f, model(), "LP Filter Resonance"),
	m_hpFilCutModel(0.0f, model(), "HP Filter Cutoff"),
	m_hpFilCutSweepModel(0.0f, model(), "HP Filter Cutoff Sweep"),
	m_waveFormModel( SQR_WAVE, 0, WAVES_NUM-1, model(), tr( "Wave" ) )
{
}




void SfxrInstrument::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	_this.setAttribute( "version", "1" );
	m_attModel.saveSettings( _doc, _this, "att" );
	m_holdModel.saveSettings( _doc, _this, "hold" );
	m_susModel.saveSettings( _doc, _this, "sus" );
	m_decModel.saveSettings( _doc, _this, "dec" );

	m_startFreqModel.saveSettings( _doc, _this, "startFreq" );
	m_minFreqModel.saveSettings( _doc, _this, "minFreq" );
	m_slideModel.saveSettings( _doc, _this, "slide" );
	m_dSlideModel.saveSettings( _doc, _this, "dSlide" );
	m_vibDepthModel.saveSettings( _doc, _this, "vibDepth" );
	m_vibSpeedModel.saveSettings( _doc, _this, "vibSpeed" );

	m_changeAmtModel.saveSettings( _doc, _this, "changeAmt" );
	m_changeSpeedModel.saveSettings( _doc, _this, "changeSpeed" );

	m_sqrDutyModel.saveSettings( _doc, _this, "sqrDuty" );
	m_sqrSweepModel.saveSettings( _doc, _this, "sqrSweep" );

	m_repeatSpeedModel.saveSettings( _doc, _this, "repeatSpeed" );

	m_phaserOffsetModel.saveSettings( _doc, _this, "phaserOffset" );
	m_phaserSweepModel.saveSettings( _doc, _this, "phaserSweep" );

	m_lpFilCutModel.saveSettings( _doc, _this, "lpFilCut" );
	m_lpFilCutSweepModel.saveSettings( _doc, _this, "lpFilCutSweep" );
	m_lpFilResoModel.saveSettings( _doc, _this, "lpFilReso" );
	m_hpFilCutModel.saveSettings( _doc, _this, "hpFilCut" );
	m_hpFilCutSweepModel.saveSettings( _doc, _this, "hpFilCutSweep" );

	m_waveFormModel.saveSettings( _doc, _this, "waveForm" );
}




void SfxrInstrument::loadSettings( const QDomElement & _this )
{

	m_attModel.loadSettings(_this, "att" );
	m_holdModel.loadSettings( _this, "hold" );
	m_susModel.loadSettings( _this, "sus" );
	m_decModel.loadSettings( _this, "dec" );

	m_startFreqModel.loadSettings( _this, "startFreq" );
	m_minFreqModel.loadSettings( _this, "minFreq" );
	m_slideModel.loadSettings( _this, "slide" );
	m_dSlideModel.loadSettings( _this, "dSlide" );
	m_vibDepthModel.loadSettings( _this, "vibDepth" );
	m_vibSpeedModel.loadSettings( _this, "vibSpeed" );

	m_changeAmtModel.loadSettings( _this, "changeAmt" );
	m_changeSpeedModel.loadSettings( _this, "changeSpeed" );

	m_sqrDutyModel.loadSettings( _this, "sqrDuty" );
	m_sqrSweepModel.loadSettings( _this, "sqrSweep" );

	m_repeatSpeedModel.loadSettings( _this, "repeatSpeed" );

	m_phaserOffsetModel.loadSettings( _this, "phaserOffset" );
	m_phaserSweepModel.loadSettings( _this, "phaserSweep" );

	m_lpFilCutModel.loadSettings( _this, "lpFilCut" );
	m_lpFilCutSweepModel.loadSettings( _this, "lpFilCutSweep" );
	m_lpFilResoModel.loadSettings( _this, "lpFilReso" );
	m_hpFilCutModel.loadSettings( _this, "hpFilCut" );
	m_hpFilCutSweepModel.loadSettings( _this, "hpFilCutSweep" );

	m_waveFormModel.loadSettings( _this, "waveForm" );

}




QString SfxrInstrument::nodeName() const
{
	return( sfxr_plugin_descriptor.name );
}




void SfxrInstrument::playNote( NotePlayHandle * _n, sampleFrame * _working_buffer )
{
	float currentSampleRate = Engine::audioEngine()->processingSampleRate();

    fpp_t frameNum = _n->framesLeftForCurrentPeriod();
    const f_cnt_t offset = _n->noteOffset();
	if ( _n->totalFramesPlayed() == 0 || _n->m_pluginData == nullptr )
	{
		_n->m_pluginData = new SfxrSynth( this );
	}
	else if( static_cast<SfxrSynth*>(_n->m_pluginData)->isPlaying() == false )
	{
		memset(_working_buffer + offset, 0, sizeof(sampleFrame) * frameNum);
		_n->noteOff();
		return;
	}

	const auto baseFreq = instrumentTrack()->baseFreq();
	int32_t pitchedFrameNum = (_n->frequency() / baseFreq) * frameNum;

	pitchedFrameNum /= ( currentSampleRate / 44100 );

// debug code
//	qDebug( "pFN %d", pitchedFrameNum );

	auto pitchedBuffer = new sampleFrame[pitchedFrameNum];
	static_cast<SfxrSynth*>(_n->m_pluginData)->update( pitchedBuffer, pitchedFrameNum );
	for( fpp_t i=0; i<frameNum; i++ )
	{
		for( ch_cnt_t j=0; j<DEFAULT_CHANNELS; j++ )
		{
			_working_buffer[i+offset][j] = pitchedBuffer[i*pitchedFrameNum/frameNum][j];
		}
	}

	delete[] pitchedBuffer;

	applyRelease( _working_buffer, _n );

	instrumentTrack()->processAudioBuffer( _working_buffer, frameNum + offset, _n );

}




void SfxrInstrument::deleteNotePluginData( NotePlayHandle * _n )
{
	delete static_cast<SfxrSynth *>( _n->m_pluginData );
}




gui::InstrumentView * SfxrInstrument::instantiateView( QWidget * _parent )
{
	return( new gui::SfxrInstrumentView( this, _parent ) );
}




void SfxrInstrument::resetModels()
{

	m_attModel.reset();
	m_holdModel.reset();
	m_susModel.reset();
	m_decModel.reset();

	m_startFreqModel.reset();
	m_minFreqModel.reset();
	m_slideModel.reset();
	m_dSlideModel.reset();
	m_vibDepthModel.reset();
	m_vibSpeedModel.reset();

	m_changeAmtModel.reset();
	m_changeSpeedModel.reset();

	m_sqrDutyModel.reset();
	m_sqrSweepModel.reset();

	m_repeatSpeedModel.reset();

	m_phaserOffsetModel.reset();
	m_phaserSweepModel.reset();

	m_lpFilCutModel.reset();
	m_lpFilCutSweepModel.reset();
	m_lpFilResoModel.reset();
	m_hpFilCutModel.reset();
	m_hpFilCutSweepModel.reset();

	m_waveFormModel.reset();
}



namespace gui
{


class SfxrKnob : public Knob
{
public:
	SfxrKnob( FloatModel* _model, QWidget * _parent ) :
			Knob( knobStyled, _model, _parent )
	{
		setFixedSize( 20, 20 );
		setCenterPointX( 10.0 );
		setCenterPointY( 10.0 );
		setTotalAngle( 270.0 );
		setLineWidth( 1 );
	}
};




#define createKnob( _model, _knob, _x, _y, _name )\
	_knob = new SfxrKnob( _model, this ); \
	_knob->setHintText( tr( _name ":" ), "" ); \
	_knob->move( _x, _y ); \
	_knob->setToolTip(tr(_name));




#define createButton( _button, _x, _y, _name, _resName )\
	_button = new PixmapButton( this, tr( _name ) );\
	_button->move( _x, _y );\
	_button->setActiveGraphic( embed::getIconPixmap( _resName "_active" ) );\
	_button->setInactiveGraphic( embed::getIconPixmap( _resName "_inactive" ) );\
	_button->setToolTip(tr(_name));




#define createButtonLocalGraphic( _button, _x, _y, _name, _resName )\
	_button = new PixmapButton( this, new BoolModel(false, this), tr( _name ) );\
	_button->move( _x, _y );\
	_button->setActiveGraphic( PLUGIN_NAME::getIconPixmap( _resName "_active" ) );\
	_button->setInactiveGraphic( PLUGIN_NAME::getIconPixmap( _resName "_inactive" ) );\
	_button->setToolTip(tr(_name));




SfxrInstrumentView::SfxrInstrumentView( SfxrInstrument * _instrument,
					QWidget * _parent ) :
	InstrumentViewImpl( _instrument, _parent, true )
{
	srand(time(nullptr));
	setAutoFillBackground( true );
	QPalette pal;
	pal.setBrush( backgroundRole(), PLUGIN_NAME::getIconPixmap( "artwork" ) );
	setPalette( pal );

	createKnob(&m_instrument->m_attModel, m_attKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*0, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*0, "Attack Time");
	createKnob(&m_instrument->m_holdModel, m_holdKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*1, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*0, "Sustain Time");
	createKnob(&m_instrument->m_susModel, m_susKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*2, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*0, "Sustain Punch");
	createKnob(&m_instrument->m_decModel, m_decKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*3, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*0, "Decay Time");

	m_attKnob	->setObjectName( "envKnob" );
	m_holdKnob	->setObjectName( "envKnob" );
	m_susKnob	->setObjectName( "envKnob" );
	m_decKnob	->setObjectName( "envKnob" );

	createKnob(&m_instrument->m_startFreqModel, m_startFreqKnob,	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*0, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*1, "Start Frequency");
	createKnob(&m_instrument->m_minFreqModel, m_minFreqKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*1, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*1, "Min Frequency");
	createKnob(&m_instrument->m_slideModel, m_slideKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*2, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*1, "Slide");
	createKnob(&m_instrument->m_dSlideModel, m_dSlideKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*3, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*1, "Delta Slide");
	createKnob(&m_instrument->m_vibDepthModel, m_vibDepthKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*4, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*1, "Vibrato Depth");
	createKnob(&m_instrument->m_vibSpeedModel, m_vibSpeedKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*5, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*1, "Vibrato Speed");

	m_startFreqKnob	->setObjectName( "freqKnob" );
	m_minFreqKnob	->setObjectName( "freqKnob" );
	m_slideKnob		->setObjectName( "freqKnob" );
	m_dSlideKnob	->setObjectName( "freqKnob" );
	m_vibDepthKnob	->setObjectName( "freqKnob" );
	m_vibSpeedKnob	->setObjectName( "freqKnob" );

	createKnob(&m_instrument->m_changeAmtModel, m_changeAmtKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*0, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*2, "Change Amount");
	createKnob(&m_instrument->m_changeSpeedModel, m_changeSpeedKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*1, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*2, "Change Speed");

	m_changeAmtKnob		->setObjectName( "changeKnob" );
	m_changeSpeedKnob	->setObjectName( "changeKnob" );

	createKnob(&m_instrument->m_sqrDutyModel, m_sqrDutyKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*3, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*2, "Square Duty (Square wave only)");
    createKnob(&m_instrument->m_sqrSweepModel, m_sqrSweepKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*4, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*2, "Duty Sweep (Square wave only)");

	m_sqrDutyKnob	->setObjectName( "sqrKnob" );
	m_sqrSweepKnob	->setObjectName( "sqrKnob" );

	createKnob(&m_instrument->m_repeatSpeedModel,	m_repeatSpeedKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*0, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*3, "Repeat Speed");

	m_repeatSpeedKnob	->setObjectName( "repeatKnob" );

	createKnob(&m_instrument->m_phaserOffsetModel,	m_phaserOffsetKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*3, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*3, "Phaser Offset");
	createKnob(&m_instrument->m_phaserSweepModel, 	m_phaserSweepKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*4, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*3, "Phaser Sweep");

	m_phaserOffsetKnob	->setObjectName( "phaserKnob" );
	m_phaserSweepKnob	->setObjectName( "phaserKnob" );

	createKnob(&m_instrument->m_lpFilCutModel,	m_lpFilCutKnob,		KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*0, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*4, "LP Filter Cutoff");
	createKnob(&m_instrument->m_lpFilCutSweepModel,	m_lpFilCutSweepKnob, KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*1, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*4, "LP Filter Cutoff Sweep");
	createKnob(&m_instrument->m_lpFilResoModel,	m_lpFilResoKnob, 	KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*2, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*4, "LP Filter Resonance");
	createKnob(&m_instrument->m_hpFilCutModel,	m_hpFilCutKnob,		KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*3, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*4, "HP Filter Cutoff");
	createKnob(&m_instrument->m_hpFilCutSweepModel,	m_hpFilCutSweepKnob, KNOBS_BASE_X+KNOB_BLOCK_SIZE_X*4, KNOBS_BASE_Y+KNOB_BLOCK_SIZE_Y*4, "HP Filter Cutoff Sweep");

	m_lpFilCutKnob		->setObjectName( "filterKnob" );
	m_lpFilCutSweepKnob	->setObjectName( "filterKnob" );
	m_lpFilResoKnob		->setObjectName( "filterKnob" );
	m_hpFilCutKnob		->setObjectName( "filterKnob" );
	m_hpFilCutSweepKnob	->setObjectName( "filterKnob" );

	createButtonLocalGraphic(m_sqrWaveBtn,		KNOBS_BASE_X+WAVEFORM_BUTTON_WIDTH*0, WAVEFORM_BASE_Y, "Square Wave", "sfxr_square_wave");
	createButtonLocalGraphic(m_sawWaveBtn,		KNOBS_BASE_X+WAVEFORM_BUTTON_WIDTH*1, WAVEFORM_BASE_Y, "Saw Wave", "sfxr_saw_wave");
	createButtonLocalGraphic(m_sinWaveBtn,		KNOBS_BASE_X+WAVEFORM_BUTTON_WIDTH*2, WAVEFORM_BASE_Y, "Sine Wave", "sfxr_sin_wave");
	createButtonLocalGraphic(m_noiseWaveBtn,	KNOBS_BASE_X+WAVEFORM_BUTTON_WIDTH*3, WAVEFORM_BASE_Y, "Noise", "sfxr_white_noise_wave");

	m_waveBtnGroup = new automatableButtonGroup( &m_instrument->m_waveFormModel, this );
	m_waveBtnGroup->addButton(m_sqrWaveBtn);
	m_waveBtnGroup->addButton(m_sawWaveBtn);
	m_waveBtnGroup->addButton(m_sinWaveBtn);
    m_waveBtnGroup->addButton(m_noiseWaveBtn);


	createButtonLocalGraphic(m_pickupBtn,		GENERATOR_BASE_X+GENERATOR_BUTTON_WIDTH*0, GENERATOR_BASE_Y, "Generate pick up/coin sfx", "pickup");
	createButtonLocalGraphic(m_laserBtn,		GENERATOR_BASE_X+GENERATOR_BUTTON_WIDTH*1, GENERATOR_BASE_Y, "Generate laser/shoot sfx", "laser");
	createButtonLocalGraphic(m_explosionBtn,	GENERATOR_BASE_X+GENERATOR_BUTTON_WIDTH*2, GENERATOR_BASE_Y, "Generate explosion sfx", "explosion");
	createButtonLocalGraphic(m_powerupBtn,		GENERATOR_BASE_X+GENERATOR_BUTTON_WIDTH*3, GENERATOR_BASE_Y, "Generate power up sfx", "powerup");
	createButtonLocalGraphic(m_hitBtn,			GENERATOR_BASE_X+GENERATOR_BUTTON_WIDTH*4, GENERATOR_BASE_Y, "Generate hit/hurt sfx", "hit");
	createButtonLocalGraphic(m_jumpBtn,			GENERATOR_BASE_X+GENERATOR_BUTTON_WIDTH*5, GENERATOR_BASE_Y, "Generate jump sfx", "jump");
	createButtonLocalGraphic(m_blipBtn,			GENERATOR_BASE_X+GENERATOR_BUTTON_WIDTH*6, GENERATOR_BASE_Y, "Generate blip/select sfx", "blip");
	connect( m_pickupBtn, SIGNAL ( clicked() ), this, SLOT ( genPickup() ) );
	connect( m_laserBtn, SIGNAL ( clicked() ), this, SLOT ( genLaser() ) );
	connect( m_explosionBtn, SIGNAL ( clicked() ), this, SLOT ( genExplosion() ) );
	connect( m_powerupBtn, SIGNAL ( clicked() ), this, SLOT ( genPowerup() ) );
	connect( m_hitBtn, SIGNAL ( clicked() ), this, SLOT ( genHit() ) );
	connect( m_jumpBtn, SIGNAL ( clicked() ), this, SLOT ( genJump() ) );
	connect( m_blipBtn, SIGNAL ( clicked() ), this, SLOT ( genBlip() ) );


	createButtonLocalGraphic(m_randomizeBtn,	RAND_BUTTON_X, RAND_BUTTON_Y, "Generate random sfx", "randomize");
	createButtonLocalGraphic(m_mutateBtn,		MUTA_BUTTON_X, MUTA_BUTTON_Y, "Mutate sfx", "mutate");
	connect( m_randomizeBtn, SIGNAL ( clicked() ), this, SLOT ( randomize() ) );
	connect( m_mutateBtn, SIGNAL ( clicked() ), this, SLOT ( mutate() ) );


	//preview sound on generator/random/mutate button clicked
/*  // disabled for now	
	connect( m_pickupBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
	connect( m_laserBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
	connect( m_explosionBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
	connect( m_powerupBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
	connect( m_hitBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
	connect( m_jumpBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
	connect( m_blipBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
	connect( m_randomizeBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
	connect( m_mutateBtn, SIGNAL ( clicked() ), this, SLOT ( previewSound() ) );
*/
}



void SfxrInstrumentView::genPickup()
{
	m_instrument->resetModels();
	m_instrument->m_startFreqModel.setValue( 0.4f+frnd(0.5f) );
	m_instrument->m_attModel.setValue( 0.0f );
	m_instrument->m_holdModel.setValue( frnd(0.1f) );
	m_instrument->m_decModel.setValue( 0.1f+frnd(0.4f) );
	m_instrument->m_susModel.setValue( 0.3f+frnd(0.3f) );

	if(rnd(1))
	{
		m_instrument->m_changeSpeedModel.setValue( 0.5f+frnd(0.2f) );
		m_instrument->m_changeAmtModel.setValue( 0.2f+frnd(0.4f) );
	}
}




void SfxrInstrumentView::genLaser()
{
	m_instrument->resetModels();

	m_instrument->m_waveFormModel.setValue( rnd(2) );
	if(m_instrument->m_waveFormModel.value()==2 && rnd(1))
		m_instrument->m_waveFormModel.setValue( rnd(1) );

	m_instrument->m_startFreqModel.setValue( 0.5f+frnd(0.5f) );
	m_instrument->m_minFreqModel.setValue(	m_instrument->m_startFreqModel.value()-0.2f-frnd(0.6f)	);

	if(m_instrument->m_minFreqModel.value()<0.2f)
	{
		m_instrument->m_minFreqModel.setValue(0.2f);
	}

	m_instrument->m_slideModel.setValue( -0.15f-frnd(0.2f) );

	if(rnd(2)==0)
	{
		m_instrument->m_startFreqModel.setValue( 0.3f+frnd(0.6f) );
		m_instrument->m_minFreqModel.setValue( frnd(0.1f) );
		m_instrument->m_slideModel.setValue( -0.35f-frnd(0.3f) );
	}

	if(rnd(1))
	{
		m_instrument->m_sqrDutyModel.setValue( frnd(0.5f) );
		m_instrument->m_sqrSweepModel.setValue( 0.2f );
	}
	else
	{
		m_instrument->m_sqrDutyModel.setValue( 0.4f+frnd(0.5f) );
		m_instrument->m_sqrSweepModel.setValue( -frnd(0.7f) );
	}

	m_instrument->m_attModel.setValue( 0.0f );
	m_instrument->m_holdModel.setValue( 0.1f+frnd(0.2f) );
	m_instrument->m_decModel.setValue( frnd(0.4f) );

	if(rnd(1))
	{
		m_instrument->m_susModel.setValue( frnd(0.3f) );
	}

	if(rnd(2)==0)
	{
		m_instrument->m_phaserOffsetModel.setValue( frnd(0.2f) );
		m_instrument->m_phaserSweepModel.setValue( -frnd(0.2f) );
	}

	if(rnd(1))
		m_instrument->m_hpFilCutModel.setValue( frnd(0.3f) );
}




void SfxrInstrumentView::genExplosion()
{
	m_instrument->resetModels();

	m_instrument->m_waveFormModel.setValue( 3 );

	if(rnd(1))
	{
		m_instrument->m_startFreqModel.setValue( 0.1f+frnd(0.4f) );
		m_instrument->m_slideModel.setValue( -0.1f+frnd(0.4f) );
	}
	else
	{
		m_instrument->m_startFreqModel.setValue( 0.2f+frnd(0.7f) );
		m_instrument->m_slideModel.setValue( -0.2f-frnd(0.2f) );
	}
	m_instrument->m_startFreqModel.setValue( m_instrument->m_startFreqModel.value()*m_instrument->m_startFreqModel.value() );

	if(rnd(4)==0)
	{
		m_instrument->m_slideModel.setValue( 0.0f );
	}

	if(rnd(2)==0)
	{
		m_instrument->m_repeatSpeedModel.setValue( 0.3f+frnd(0.5f) );
	}

	m_instrument->m_attModel.setValue( 0.0f );
	m_instrument->m_holdModel.setValue( 0.1f+frnd(0.3f) );
	m_instrument->m_decModel.setValue( 0.5f );
	if(rnd(1)==0)
	{
		m_instrument->m_phaserOffsetModel.setValue( -0.3f+frnd(0.9f) );
		m_instrument->m_phaserSweepModel.setValue( -frnd(0.3f) );
	}
	m_instrument->m_susModel.setValue( 0.2f+frnd(0.6f) );

	if(rnd(1))
	{
		m_instrument->m_vibDepthModel.setValue( frnd(0.7f) );
		m_instrument->m_vibSpeedModel.setValue( frnd(0.6f) );
	}
	if(rnd(2)==0)
	{
		m_instrument->m_changeSpeedModel.setValue( 0.6f+frnd(0.3f) );
		m_instrument->m_changeAmtModel.setValue( 0.8f-frnd(1.6f) );
	}

}




void SfxrInstrumentView::genPowerup()
{
	m_instrument->resetModels();

	if(rnd(1))
		m_instrument->m_waveFormModel.setValue( 1 );
	else
		m_instrument->m_sqrDutyModel.setValue( frnd(0.6f) );
	if(rnd(1))
	{
		m_instrument->m_startFreqModel.setValue( 0.2f+frnd(0.3f) );
		m_instrument->m_slideModel.setValue( 0.1f+frnd(0.4f) );
		m_instrument->m_repeatSpeedModel.setValue( 0.4f+frnd(0.4f) );
	}
	else
	{
		m_instrument->m_startFreqModel.setValue( 0.2f+frnd(0.3f) );
		m_instrument->m_slideModel.setValue( 0.05f+frnd(0.2f) );
		if(rnd(1))
		{
			m_instrument->m_vibDepthModel.setValue( frnd(0.7f) );
			m_instrument->m_vibSpeedModel.setValue( frnd(0.6f) );
		}
	}

	m_instrument->m_attModel.setValue( 0.0f );
	m_instrument->m_holdModel.setValue( frnd(0.4f) );
	m_instrument->m_decModel.setValue( 0.1f+frnd(0.4f) );
}




void SfxrInstrumentView::genHit()
{
	m_instrument->resetModels();

	m_instrument->m_waveFormModel.setValue( rnd(2) );
	if(m_instrument->m_waveFormModel.value()==2)
	{
		m_instrument->m_waveFormModel.setValue( 3 );
	}
	if(m_instrument->m_waveFormModel.value()==0)
	{
		m_instrument->m_sqrDutyModel.setValue( frnd(0.6f) );
	}

	m_instrument->m_startFreqModel.setValue( 0.2f+frnd(0.6f) );
	m_instrument->m_slideModel.setValue( -0.3f-frnd(0.4f) );

	m_instrument->m_attModel.setValue( 0.0f );
	m_instrument->m_holdModel.setValue( frnd(0.1f) );
	m_instrument->m_decModel.setValue( 0.1f+frnd(0.2f) );
	if(rnd(1))
	{
		m_instrument->m_hpFilCutModel.setValue( frnd(0.3f) );
	}
}




void SfxrInstrumentView::genJump()
{
	m_instrument->resetModels();

	m_instrument->m_waveFormModel.setValue( 0 );
	m_instrument->m_sqrDutyModel.setValue( frnd(0.6f) );

	m_instrument->m_startFreqModel.setValue( 0.3f+frnd(0.3f) );
	m_instrument->m_slideModel.setValue( 0.1f+frnd(0.2f) );

	m_instrument->m_attModel.setValue( 0.0f );
	m_instrument->m_holdModel.setValue( 0.1f+frnd(0.3f) );
	m_instrument->m_decModel.setValue( 0.1f+frnd(0.2f) );

	if(rnd(1))
	{
		m_instrument->m_hpFilCutModel.setValue( frnd(0.3f) );
	}
	if(rnd(1))
	{

		m_instrument->m_lpFilCutModel.setValue( 1.0f-frnd(0.6f) );
	}

}




void SfxrInstrumentView::genBlip()
{
	m_instrument->resetModels();

	m_instrument->m_waveFormModel.setValue( rnd(1) );
	if( m_instrument->m_waveFormModel.value()==0 )
	{
		m_instrument->m_sqrDutyModel.setValue( frnd(0.6f) );
	}

	m_instrument->m_startFreqModel.setValue( 0.2f+frnd(0.4f) );

	m_instrument->m_attModel.setValue( 0.0f );
	m_instrument->m_holdModel.setValue( 0.1f+frnd(0.1f) );
	m_instrument->m_decModel.setValue( frnd(0.2f) );
	m_instrument->m_hpFilCutModel.setValue( 0.1f );
}




void SfxrInstrumentView::randomize()
{
	m_instrument->m_startFreqModel.setValue( pow(frnd(2.0f)-1.0f, 2.0f) );
	if(rnd(1))
	{
		m_instrument->m_startFreqModel.setValue( pow(frnd(2.0f)-1.0f, 3.0f)+0.5f );
	}
	m_instrument->m_minFreqModel.setValue( 0.0f );
	m_instrument->m_slideModel.setValue( pow(frnd(2.0f)-1.0f, 5.0f) );
	if( m_instrument->m_startFreqModel.value()>0.7f && m_instrument->m_slideModel.value()>0.2f )
	{
		m_instrument->m_slideModel.setValue( -m_instrument->m_slideModel.value() );
	}
	if( m_instrument->m_startFreqModel.value()<0.2f && m_instrument->m_slideModel.value()<-0.05f )
	{
		m_instrument->m_slideModel.setValue( -m_instrument->m_slideModel.value() );
	}
	m_instrument->m_dSlideModel.setValue( pow(frnd(2.0f)-1.0f, 3.0f) );

	m_instrument->m_sqrDutyModel.setValue( frnd(2.0f)-1.0f );
	m_instrument->m_sqrSweepModel.setValue( pow(frnd(2.0f)-1.0f, 3.0f) );

	m_instrument->m_vibDepthModel.setValue( pow(frnd(2.0f)-1.0f, 3.0f) );
	m_instrument->m_vibSpeedModel.setValue( frnd(2.0f)-1.0f );
	//m_instrument->m_vibDelayModel.setValue( frnd(2.0f)-1.0f );

	m_instrument->m_attModel.setValue( pow(frnd(2.0f)-1.0f, 3.0f) );
	m_instrument->m_holdModel.setValue( pow(frnd(2.0f)-1.0f, 2.0f) );
	m_instrument->m_decModel.setValue( frnd(2.0f)-1.0f );
	m_instrument->m_susModel.setValue( pow(frnd(0.8f), 2.0f) );
	if(m_instrument->m_attModel.value()+m_instrument->m_holdModel.value()+m_instrument->m_decModel.value()<0.2f)
	{
		m_instrument->m_holdModel.setValue( m_instrument->m_holdModel.value()+0.2f+frnd(0.3f) );
		m_instrument->m_decModel.setValue( m_instrument->m_decModel.value()+0.2f+frnd(0.3f) );
	}

	m_instrument->m_lpFilResoModel.setValue( frnd(2.0f)-1.0f );
	m_instrument->m_lpFilCutModel.setValue( 1.0f-pow(frnd(1.0f), 3.0f) );
	m_instrument->m_lpFilCutSweepModel.setValue( pow(frnd(2.0f)-1.0f, 3.0f) );
	if(m_instrument->m_lpFilCutModel.value()<0.1f && m_instrument->m_lpFilCutSweepModel.value()<-0.05f)
	{
		m_instrument->m_lpFilCutSweepModel.setValue( -m_instrument->m_lpFilCutSweepModel.value() );
	}
	m_instrument->m_hpFilCutModel.setValue( pow(frnd(1.0f), 5.0f) );
	m_instrument->m_hpFilCutSweepModel.setValue( pow(frnd(2.0f)-1.0f, 5.0f) );

	m_instrument->m_phaserOffsetModel.setValue( pow(frnd(2.0f)-1.0f, 3.0f) );
	m_instrument->m_phaserSweepModel.setValue( pow(frnd(2.0f)-1.0f, 3.0f) );

	m_instrument->m_repeatSpeedModel.setValue( frnd(2.0f)-1.0f );

	m_instrument->m_changeSpeedModel.setValue( frnd(2.0f)-1.0f );
	m_instrument->m_changeAmtModel.setValue( frnd(2.0f)-1.0f );

}




void SfxrInstrumentView::mutate()
{
	if(rnd(1)) m_instrument->m_startFreqModel.setValue( m_instrument->m_startFreqModel.value()+frnd(0.1f)-0.05f );
	//		if(rnd(1)) m_instrument->m_minFreqModel.setValue( m_instrument->m_minFreqModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_slideModel.setValue( m_instrument->m_slideModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_dSlideModel.setValue( m_instrument->m_dSlideModel.value()+frnd(0.1f)-0.05f );

	if(rnd(1)) m_instrument->m_sqrDutyModel.setValue( m_instrument->m_sqrDutyModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_sqrSweepModel.setValue( m_instrument->m_sqrSweepModel.value()+frnd(0.1f)-0.05f );

	if(rnd(1)) m_instrument->m_vibDepthModel.setValue( m_instrument->m_vibDepthModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_vibSpeedModel.setValue( m_instrument->m_vibSpeedModel.value()+frnd(0.1f)-0.05f );
	//		if(rnd(1)) m_instrument->m_vibDelayModel.setValue( m_instrument->m_vibDelayModel.value()+frnd(0.1f)-0.05f );

	if(rnd(1)) m_instrument->m_attModel.setValue( m_instrument->m_attModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_holdModel.setValue( m_instrument->m_holdModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_decModel.setValue( m_instrument->m_decModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_susModel.setValue( m_instrument->m_susModel.value()+frnd(0.1f)-0.05f );

	if(rnd(1)) m_instrument->m_lpFilResoModel.setValue( m_instrument->m_lpFilResoModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_lpFilCutModel.setValue( m_instrument->m_lpFilCutModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_lpFilCutSweepModel.setValue( m_instrument->m_lpFilCutSweepModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_hpFilCutModel.setValue( m_instrument->m_hpFilCutModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_hpFilCutSweepModel.setValue( m_instrument->m_hpFilCutSweepModel.value()+frnd(0.1f)-0.05f );

	if(rnd(1)) m_instrument->m_phaserOffsetModel.setValue( m_instrument->m_phaserOffsetModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_phaserSweepModel.setValue( m_instrument->m_phaserSweepModel.value()+frnd(0.1f)-0.05f );

	if(rnd(1)) m_instrument->m_repeatSpeedModel.setValue( m_instrument->m_repeatSpeedModel.value()+frnd(0.1f)-0.05f );

	if(rnd(1)) m_instrument->m_changeSpeedModel.setValue( m_instrument->m_changeSpeedModel.value()+frnd(0.1f)-0.05f );
	if(rnd(1)) m_instrument->m_changeAmtModel.setValue( m_instrument->m_changeAmtModel.value()+frnd(0.1f)-0.05f );

}




void SfxrInstrumentView::previewSound()
{
	InstrumentTrack* it = m_instrument->instrumentTrack();
	it->silenceAllNotes();
	it->processInEvent( MidiEvent( MidiNoteOn, 0, it->baseNoteModel()->value(), MidiDefaultVelocity ) );
}



} // namespace gui


extern "C"
{

// necessary for getting instance out of shared lib
PLUGIN_EXPORT Plugin * lmms_plugin_main( Model* m, void* )
{
	return new SfxrInstrument( static_cast<InstrumentTrack *>( m ) );
}


}


} // namespace lmms
