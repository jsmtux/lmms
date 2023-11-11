/*
 * Kicker.cpp - drum synthesizer
 *
 * Copyright (c) 2006-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2014 grejppi <grejppi/at/gmail.com>
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

#include "Kicker.h"
#include "KickerGui.h"
#include "IAudioEngine.h"
#include "IEngine.h"
#include "ITrack.h"
#include "IPlayHandle.h"
#include "KickerOsc.h"
#include "IModels.h"
#include "instrument/InstrumentView.h"

#include "embed.h"
#include "plugin_export.h"

#include "widgets/Knob.h"
#include "widgets/LedCheckBox.h"
#include "widgets/TempoSyncKnob.h"

namespace lmms
{


extern "C"
{

PluginDescriptor PLUGIN_EXPORT kicker_plugin_descriptor =
{
	LMMS_STRINGIFY( PLUGIN_NAME ),
	KickerInstrument::PluginName(),
	QT_TRANSLATE_NOOP( "PluginBrowser",
				"Versatile drum synthesizer" ),
	"Tobias Doerffel <tobydox/at/users.sf.net>",
	0x0100,
	PluginTypes::Instrument,
	new PluginPixmapLoader( "logo" ),
	nullptr,
	nullptr,
} ;

}

namespace gui
{


gui::PluginView * KickerQWidgetIntrument::createView( QWidget * parent )
{
	return new gui::KickerInstrumentView( this, parent );
}


class KickerKnob : public Knob
{
public:
	KickerKnob( IFloatAutomatableModel* _model, QWidget * _parent ) :
			Knob( knobStyled, _model, _parent )
	{
		setFixedSize( 29, 29 );
		setObjectName( "smallKnob" );
	}
};


class KickerEnvKnob : public TempoSyncKnob
{
public:
	KickerEnvKnob( ITempoSyncKnobModelWrapper* _model, QWidget * _parent ) :
			TempoSyncKnob( knobStyled, _model, _parent )
	{
		setFixedSize( 29, 29 );
		setObjectName( "smallKnob" );
	}
};


class KickerLargeKnob : public Knob
{
public:
	KickerLargeKnob( IFloatAutomatableModel* _model, QWidget * _parent ) :
			Knob( knobStyled, _model, _parent )
	{
		setFixedSize( 34, 34 );
		setObjectName( "largeKnob" );
	}
};




KickerInstrumentView::KickerInstrumentView( KickerInstrument * _instrument,
							QWidget * _parent ) :
	InstrumentViewImpl( _instrument, _parent, true )
{
	const int ROW1 = 14;
	const int ROW2 = ROW1 + 56;
	const int ROW3 = ROW2 + 56;
	const int LED_ROW = 63;
	const int COL1 = 14;
	const int COL2 = COL1 + 56;
	const int COL3 = COL2 + 56;
	const int COL4 = COL3 + 41;
	const int COL5 = COL4 + 41;
	const int END_COL = COL1 + 48;
	
	m_startFreqKnob = new KickerLargeKnob(  m_instrument->m_startFreqModel, this );
	m_startFreqKnob->setHintText( tr( "Start frequency:" ), "Hz" );
	m_startFreqKnob->move( COL1, ROW1 );

	m_endFreqKnob = new KickerLargeKnob( m_instrument->m_endFreqModel, this );
	m_endFreqKnob->setHintText( tr( "End frequency:" ), "Hz" );
	m_endFreqKnob->move( END_COL, ROW1 );

	m_slopeKnob = new KickerKnob( m_instrument->m_slopeModel, this );
	m_slopeKnob->setHintText( tr( "Frequency slope:" ), "" );
	m_slopeKnob->move( COL3, ROW1 );

	m_gainKnob = new KickerKnob( m_instrument->m_gainModel, this );
	m_gainKnob->setHintText( tr( "Gain:" ), "" );
	m_gainKnob->move( COL1, ROW3 );

	m_decayKnob = new KickerEnvKnob( m_instrument->m_decayModel, this );
	m_decayKnob->setHintText( tr( "Envelope length:" ), "ms" );
	m_decayKnob->move( COL2, ROW3 );

	m_envKnob = new KickerKnob( m_instrument->m_envModel, this );
	m_envKnob->setHintText( tr( "Envelope slope:" ), "" );
	m_envKnob->move( COL3, ROW3 );

	m_clickKnob = new KickerKnob( m_instrument->m_clickModel, this );
	m_clickKnob->setHintText( tr( "Click:" ), "" );
	m_clickKnob->move( COL5, ROW1 );

	m_noiseKnob = new KickerKnob( m_instrument->m_noiseModel, this );
	m_noiseKnob->setHintText( tr( "Noise:" ), "" );
	m_noiseKnob->move( COL5, ROW3 );

	m_distKnob = new KickerKnob( m_instrument->m_distModel, this );
	m_distKnob->setHintText( tr( "Start distortion:" ), "" );
	m_distKnob->move( COL4, ROW2 );

	m_distEndKnob = new KickerKnob( m_instrument->m_distEndModel, this );
	m_distEndKnob->setHintText( tr( "End distortion:" ), "" );
	m_distEndKnob->move( COL5, ROW2 );

	m_startNoteToggle = new LedCheckBox( "", m_instrument->m_startNoteModel, this, "", LedCheckBox::Green );
	m_startNoteToggle->move( COL1 + 8, LED_ROW );

	m_endNoteToggle = new LedCheckBox( "", m_instrument->m_endNoteModel, this, "", LedCheckBox::Green );
	m_endNoteToggle->move( END_COL + 8, LED_ROW );

	setAutoFillBackground( true );
	QPalette pal;
	pal.setBrush( backgroundRole(), PLUGIN_NAME::getIconPixmap( "artwork" ) );
	setPalette( pal );
}


} // namespace gui


extern "C"
{

// necessary for getting instance out of shared lib
PLUGIN_EXPORT Plugin * lmms_plugin_main( void * m, void * )
{
	return new gui::KickerQWidgetIntrument( static_cast<IInstrumentTrack *>( m ) );
}


}


} // namespace lmms
