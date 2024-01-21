/*
 * InstrumentTrackView.cpp - implementation of InstrumentTrackView class
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

#include "InstrumentTrackView.h"

#include "IAudioEngine.h"
#include "IConfigManager.h"
#include "IEngine.h"
#include "FadeButton.h"
#include "GuiApplication.h"
#include "ITrack.h"
#include "MidiCCRackView.h"
#include "IMixer.h"
#include "MixerView.h"
#include "MainWindow.h"
#include "IMidiClient.h"
#include "TrackLabelButton.h"

#include "instrument/InstrumentTrackWindow.h"
#include "menus/MidiPortMenu.h"
#include "widgets/Knob.h"

#include <QAction>
#include <QApplication>
#include <QDragEnterEvent>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>


namespace lmms::gui
{


InstrumentTrackView::InstrumentTrackView( IInstrumentTrack * _it, TrackContainerView* tcv ) :
	TrackView( _it->baseTrack(), tcv ),
	m_instrumentTrack(_it),
	m_window( nullptr ),
	m_lastPos( -1, -1 )
{
	setAcceptDrops( true );
	setFixedHeight( 32 );

	m_tlb = new TrackLabelButton( this, getTrackSettingsWidget() );
	m_tlb->setCheckable( true );
	m_tlb->setIcon( embed::getIconPixmap( "instrument_track" ) );
	m_tlb->move( 3, 1 );
	m_tlb->show();

	connect( m_tlb, SIGNAL(toggled(bool)),
			this, SLOT(toggleInstrumentWindow(bool)));

	connect( _it->baseTrack()->model(), SIGNAL(nameChanged()),
			m_tlb, SLOT(update()));

	connect(IConfigManager::Instance(), SIGNAL(valueChanged(QString,QString,QString)),
			this, SLOT(handleConfigChange(QString,QString,QString)));

	// creation of widgets for track-settings-widget
	int widgetWidth;
	if( IConfigManager::Instance()->value( "ui",
					  "compacttrackbuttons" ).toInt() )
	{
		widgetWidth = DEFAULT_SETTINGS_WIDGET_WIDTH_COMPACT;
	}
	else
	{
		widgetWidth = DEFAULT_SETTINGS_WIDGET_WIDTH;
	}

	m_volumeKnob = new Knob( knobSmall_17,  _it->volumeModel(), getTrackSettingsWidget(),
							tr( "Volume" ) );
	m_volumeKnob->setVolumeKnob( true );
	m_volumeKnob->setHintText( tr( "Volume:" ), "%" );
	m_volumeKnob->move( widgetWidth-2*24, 2 );
	m_volumeKnob->setLabel( tr( "VOL" ) );
	m_volumeKnob->show();

	m_panningKnob = new Knob( knobSmall_17, _it->panningModel(), getTrackSettingsWidget(),
							tr( "Panning" ) );
	m_panningKnob->setHintText(tr("Panning:"), "%");
	m_panningKnob->move( widgetWidth-24, 2 );
	m_panningKnob->setLabel( tr( "PAN" ) );
	m_panningKnob->show();

	m_midiMenu = new QMenu( tr( "MIDI" ), this );

	// sequenced MIDI?
	if( !IEngine::Instance()->getAudioEngineInterface()->midiClientInterface()->isRaw() )
	{
		_it->midiPortInterface()->setReadablePortsMenu( new MidiPortMenu(
							IMidiPort::Input, _it->midiPortInterface()));
		_it->midiPortInterface()->setWritablePortsMenu( new MidiPortMenu(
							IMidiPort::Output, _it->midiPortInterface()));
		m_midiInputAction = m_midiMenu->addMenu(
					_it->midiPortInterface()->readablePortsMenu() );
		m_midiOutputAction = m_midiMenu->addMenu(
					_it->midiPortInterface()->writablePortsMenu() );
	}
	else
	{
		m_midiInputAction = m_midiMenu->addAction( "" );
		m_midiOutputAction = m_midiMenu->addAction( "" );
		m_midiInputAction->setCheckable( true );
		m_midiOutputAction->setCheckable( true );
		connect( m_midiInputAction, SIGNAL(changed()), this,
						SLOT(midiInSelected()));
		connect( m_midiOutputAction, SIGNAL(changed()), this,
					SLOT(midiOutSelected()));
		connect( _it->midiPortInterface()->midiPortModel(), SIGNAL(modeChanged()),
				this, SLOT(midiConfigChanged()));
	}

	m_midiInputAction->setText( tr( "Input" ) );
	m_midiOutputAction->setText( tr( "Output" ) );

	QAction *midiRackAction = m_midiMenu->addAction(tr("Open/Close MIDI CC Rack"));
	midiRackAction->setIcon(embed::getIconPixmap("midi_cc_rack"));
	connect(midiRackAction, SIGNAL(triggered()),
		this, SLOT(toggleMidiCCRack()));

	m_activityIndicator = new FadeButton( QApplication::palette().color( QPalette::Active,
							QPalette::Window),
						QApplication::palette().color( QPalette::Active,
							QPalette::BrightText ),
						QApplication::palette().color( QPalette::Active,
							QPalette::BrightText).darker(),
						getTrackSettingsWidget() );
	m_activityIndicator->setGeometry(
					 widgetWidth-2*24-11, 2, 8, 28 );
	m_activityIndicator->show();
	connect( m_activityIndicator, SIGNAL(pressed()),
				this, SLOT(activityIndicatorPressed()));
	connect( m_activityIndicator, SIGNAL(released()),
				this, SLOT(activityIndicatorReleased()));
	connect( _it->instrumentTrackModel(), &InstrumentTrackModel::newNote,
			 m_activityIndicator, &FadeButton::activate);
	connect( _it->instrumentTrackModel(), &InstrumentTrackModel::endNote,
	 		m_activityIndicator, &FadeButton::noteEnd);
}




InstrumentTrackView::~InstrumentTrackView()
{
	delete m_window;
	m_window = nullptr;

	delete model()->midiPortInterface()->readablePortsMenu();
	delete model()->midiPortInterface()->writablePortsMenu();
}




void InstrumentTrackView::toggleMidiCCRack()
{
	// Lazy creation: midiCCRackView is only created when accessed the first time.
	// this->model() returns pointer to the InstrumentTrack who owns this InstrumentTrackView.
	if (!m_midiCCRackView)
	{
		m_midiCCRackView = std::unique_ptr<MidiCCRackView>(new MidiCCRackView(this->model()));
	}

	if (m_midiCCRackView->parentWidget()->isVisible())
	{
		m_midiCCRackView->parentWidget()->hide();
	}
	else
	{
		m_midiCCRackView->parentWidget()->show();
		m_midiCCRackView->show();
	}
}




InstrumentTrackWindow * InstrumentTrackView::topLevelInstrumentTrackWindow()
{
	InstrumentTrackWindow * w = nullptr;
	for( const QMdiSubWindow * sw :
				getGUI()->mainWindow()->workspace()->subWindowList(
											QMdiArea::ActivationHistoryOrder ) )
	{
		if( sw->isVisible() && sw->widget()->inherits( "lmms::gui::InstrumentTrackWindow" ) )
		{
			w = qobject_cast<InstrumentTrackWindow *>( sw->widget() );
		}
	}

	return w;
}




/*! \brief Create and assign a new mixer Channel for this track */
void InstrumentTrackView::createMixerLine()
{
	int channelIndex = getGUI()->mixerView()->addNewChannel();
	auto channel = IEngine::Instance()->getMixerInterface()->getMixerChannelInterface(channelIndex);

	channel->setName(getTrack()->name());
	if (getTrack()->useColor()) { channel->setColor (getTrack()->color()); }

	assignMixerLine(channelIndex);
}




/*! \brief Assign a specific mixer Channel for this track */
void InstrumentTrackView::assignMixerLine(int channelIndex)
{
	model()->mixerChannelModel()->setValue( channelIndex );

	getGUI()->mixerView()->setCurrentMixerLine( channelIndex );
}



InstrumentTrackWindow * InstrumentTrackView::getInstrumentTrackWindow()
{
	if (!m_window)
	{
		m_window = new InstrumentTrackWindow(this);
	}

	return m_window;
}

void InstrumentTrackView::handleConfigChange(QString cls, QString attr, QString value)
{
	// When one instrument track window mode is turned on,
	// close windows except last opened one.
	if (cls == "ui" && attr == "oneinstrumenttrackwindow" && value.toInt())
	{
		m_tlb->setChecked(m_window && m_window == topLevelInstrumentTrackWindow());
	}
}

void InstrumentTrackView::dragEnterEvent( QDragEnterEvent * _dee )
{
	InstrumentTrackWindow::dragEnterEventGeneric( _dee );
	if( !_dee->isAccepted() )
	{
		TrackView::dragEnterEvent( _dee );
	}
}




void InstrumentTrackView::dropEvent( QDropEvent * _de )
{
	getInstrumentTrackWindow()->dropEvent( _de );
	TrackView::dropEvent( _de );
}




void InstrumentTrackView::toggleInstrumentWindow( bool _on )
{
	if (_on && IConfigManager::Instance()->value("ui", "oneinstrumenttrackwindow").toInt())
	{
		if (topLevelInstrumentTrackWindow())
		{
			topLevelInstrumentTrackWindow()->m_itv->m_tlb->setChecked(false);
		}
	}

	getInstrumentTrackWindow()->toggleVisibility( _on );
}




void InstrumentTrackView::activityIndicatorPressed()
{
	model()->processInEvent( *createMidiEvent( MidiNoteOn, 0, DefaultKey, MidiDefaultVelocity ) );
}




void InstrumentTrackView::activityIndicatorReleased()
{
	model()->processInEvent( *createMidiEvent( MidiNoteOff, 0, DefaultKey, 0 ) );
}





void InstrumentTrackView::midiInSelected()
{
	if( model() )
	{
		model()->midiPortInterface()->setReadable( m_midiInputAction->isChecked() );
	}
}




void InstrumentTrackView::midiOutSelected()
{
	if( model() )
	{
		model()->midiPortInterface()->setWritable( m_midiOutputAction->isChecked() );
	}
}




void InstrumentTrackView::midiConfigChanged()
{
	m_midiInputAction->setChecked( model()->midiPortInterface()->isReadable() );
	m_midiOutputAction->setChecked( model()->midiPortInterface()->isWritable() );
}




//FIXME: This is identical to SampleTrackView::createMixerMenu
QMenu * InstrumentTrackView::createMixerMenu(QString title, QString newMixerLabel)
{
	int channelIndex = model()->mixerChannelModel()->value();

	IMixerChannel *mixerChannel = IEngine::Instance()->getMixerInterface()->getMixerChannelInterface( channelIndex );

	// If title allows interpolation, pass channel index and name
	if ( title.contains( "%2" ) )
	{
		title = title.arg( channelIndex ).arg( mixerChannel->getName() );
	}

	auto mixerMenu = new QMenu(title);

	mixerMenu->addAction( newMixerLabel, this, SLOT(createMixerLine()));
	mixerMenu->addSeparator();

	for (int i = 0; i < IEngine::Instance()->getMixerInterface()->numChannels(); ++i)
	{
		IMixerChannel * currentChannel = IEngine::Instance()->getMixerInterface()->getMixerChannelInterface( i );

		if ( currentChannel != mixerChannel )
		{
			auto index = currentChannel->channelIndex();
			QString label = tr( "%1: %2" ).arg( currentChannel->channelIndex() ).arg( currentChannel->getName() );
			mixerMenu->addAction(label, [this, index](){
				assignMixerLine(index);
			});
		}
	}

	return mixerMenu;
}


} // namespace lmms::gui
