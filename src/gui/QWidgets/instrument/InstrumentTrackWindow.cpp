/*
 * InstrumentTrackWindow.cpp - implementation of InstrumentTrackWindow class
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

#include "InstrumentTrackWindow.h"

#include "IConfigManager.h"
#include "IDataFile.h"
#include "EffectRackView.h"
#include "embed.h"
#include "IEngine.h"
#include "FileBrowser.h"
#include "GuiApplication.h"
#include "gui_templates.h"
#include "IPlugin.h"
#include "InstrumentFunctionViews.h"
#include "InstrumentMidiIOView.h"
#include "InstrumentMiscView.h"
#include "InstrumentSoundShapingView.h"
#include "ITrack.h"
#include "MainWindow.h"
#include "PianoView.h"
#include "IPluginFactory.h"
#include "PluginView.h"
#include "ISong.h"
#include "StringPairDrag.h"
#include "SubWindow.h"
#include "IMicrotuner.h"
#include "plugins/interface/IQWidgetInstrumentPlugin.h"

#include "editors/TrackContainerView.h"

#include "modals/FileDialog.h"

#include "tracks/TrackLabelButton.h"
#include "tracks/InstrumentTrackView.h"

#include "widgets/ComboBox.h"
#include "widgets/GroupBox.h"
#include "widgets/Knob.h"
#include "widgets/LcdSpinBox.h"
#include "widgets/LedCheckBox.h"
#include "widgets/LeftRightNav.h"
#include "widgets/MixerLineLcdSpinBox.h"
#include "widgets/TabWidget.h"

#include <QDir>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>

namespace lmms::gui
{


const int INSTRUMENT_WIDTH	= 254;
const int INSTRUMENT_HEIGHT	= INSTRUMENT_WIDTH;
const int PIANO_HEIGHT		= 80;


InstrumentTrackWindow::InstrumentTrackWindow( InstrumentTrackView * _itv ) :
	QWidget(),
	m_track( _itv->model() ),
	m_itv( _itv ),
	m_instrumentView( nullptr )
{
	setAcceptDrops( true );

	// init own layout + widgets
	setFocusPolicy( Qt::StrongFocus );
	auto vlayout = new QVBoxLayout(this);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing( 0 );

	auto generalSettingsWidget = new TabWidget(tr("GENERAL SETTINGS"), this);

	auto generalSettingsLayout = new QVBoxLayout(generalSettingsWidget);

	generalSettingsLayout->setContentsMargins( 8, 18, 8, 8 );
	generalSettingsLayout->setSpacing( 6 );

	auto nameAndChangeTrackWidget = new QWidget(generalSettingsWidget);
	auto nameAndChangeTrackLayout = new QHBoxLayout(nameAndChangeTrackWidget);
	nameAndChangeTrackLayout->setContentsMargins( 0, 0, 0, 0 );
	nameAndChangeTrackLayout->setSpacing( 2 );

	// setup line edit for changing instrument track name
	m_nameLineEdit = new QLineEdit;
	m_nameLineEdit->setFont( pointSize<9>( m_nameLineEdit->font() ) );
	connect( m_nameLineEdit, SIGNAL( textChanged( const QString& ) ),
				this, SLOT( textChanged( const QString& ) ) );

	m_nameLineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
	nameAndChangeTrackLayout->addWidget(m_nameLineEdit, 1);


	// set up left/right arrows for changing instrument
	m_leftRightNav = new LeftRightNav(this);
	connect( m_leftRightNav, SIGNAL(onNavLeft()), this,
						SLOT(viewPrevInstrument()));
	connect( m_leftRightNav, SIGNAL(onNavRight()), this,
						SLOT(viewNextInstrument()));
	// m_leftRightNav->setShortcuts();
	nameAndChangeTrackLayout->addWidget(m_leftRightNav);


	generalSettingsLayout->addWidget( nameAndChangeTrackWidget );

	auto basicControlsLayout = new QGridLayout;
	basicControlsLayout->setHorizontalSpacing(3);
	basicControlsLayout->setVerticalSpacing(0);
	basicControlsLayout->setContentsMargins(0, 0, 0, 0);

#if QT_VERSION < 0x50C00
	// Workaround for a bug in Qt versions below 5.12,
	// where argument-dependent-lookup fails for QFlags operators
	// declared inside a namepsace.
	// This affects the Q_DECLARE_OPERATORS_FOR_FLAGS macro in Instrument.h
	// See also: https://codereview.qt-project.org/c/qt/qtbase/+/225348

	using ::operator|;

#endif

	QString labelStyleSheet = "font-size: 6pt;";
	Qt::Alignment labelAlignment = Qt::AlignHCenter | Qt::AlignTop;
	Qt::Alignment widgetAlignment = Qt::AlignHCenter | Qt::AlignCenter;

	// set up volume knob
	m_volumeKnob = new Knob( knobBright_26, m_track->volumeModel(), nullptr, tr( "Volume" ) );
	m_volumeKnob->setVolumeKnob( true );
	m_volumeKnob->setHintText( tr( "Volume:" ), "%" );

	basicControlsLayout->addWidget( m_volumeKnob, 0, 0 );
	basicControlsLayout->setAlignment( m_volumeKnob, widgetAlignment );

	auto label = new QLabel(tr("VOL"), this);
	label->setStyleSheet( labelStyleSheet );
	basicControlsLayout->addWidget( label, 1, 0);
	basicControlsLayout->setAlignment( label, labelAlignment );


	// set up panning knob
	m_panningKnob = new Knob( knobBright_26, m_track->panningModel(), nullptr, tr( "Panning" ) );
	m_panningKnob->setHintText( tr( "Panning:" ), "" );

	basicControlsLayout->addWidget( m_panningKnob, 0, 1 );
	basicControlsLayout->setAlignment( m_panningKnob, widgetAlignment );

	label = new QLabel( tr( "PAN" ), this );
	label->setStyleSheet( labelStyleSheet );
	basicControlsLayout->addWidget( label, 1, 1);
	basicControlsLayout->setAlignment( label, labelAlignment );


	basicControlsLayout->setColumnStretch(2, 1);


	// set up pitch knob
	if( m_track->instrument() && m_track->instrument()->flags().testFlag( IInstrument::IsNotBendable ) == false )
	{
		m_pitchKnob = new Knob( knobBright_26, m_track->pitchModel(), nullptr, tr( "Pitch" ) );
		m_pitchKnob->setHintText( tr( "Pitch:" ), " " + tr( "cents" ) );

		basicControlsLayout->addWidget( m_pitchKnob, 0, 3 );
		basicControlsLayout->setAlignment( m_pitchKnob, widgetAlignment );

		m_pitchLabel = new QLabel( tr( "PITCH" ), this );
		m_pitchLabel->setStyleSheet( labelStyleSheet );
		basicControlsLayout->addWidget( m_pitchLabel, 1, 3);
		basicControlsLayout->setAlignment( m_pitchLabel, labelAlignment );

		// set up pitch range knob
		m_pitchRangeSpinBox= new LcdSpinBox( 2, m_track->pitchRangeModel(), nullptr, tr( "Pitch range (semitones)" ) );

		basicControlsLayout->addWidget( m_pitchRangeSpinBox, 0, 4 );
		basicControlsLayout->setAlignment( m_pitchRangeSpinBox, widgetAlignment );

		m_pitchRangeLabel = new QLabel( tr( "RANGE" ), this );
		m_pitchRangeLabel->setStyleSheet( labelStyleSheet );
		basicControlsLayout->addWidget( m_pitchRangeLabel, 1, 4);
		basicControlsLayout->setAlignment( m_pitchRangeLabel, labelAlignment );
	}


	basicControlsLayout->setColumnStretch(5, 1);


	// setup spinbox for selecting Mixer-channel
	m_mixerChannelNumber = new MixerLineLcdSpinBox( 2, m_track->mixerChannelModel(), nullptr, tr( "Mixer channel" ), m_itv );

	basicControlsLayout->addWidget( m_mixerChannelNumber, 0, 6 );
	basicControlsLayout->setAlignment( m_mixerChannelNumber, widgetAlignment );

	label = new QLabel( tr( "CHANNEL" ), this );
	label->setStyleSheet( labelStyleSheet );
	basicControlsLayout->addWidget( label, 1, 6);
	basicControlsLayout->setAlignment( label, labelAlignment );

	auto saveSettingsBtn = new QPushButton(embed::getIconPixmap("project_save"), QString());
	saveSettingsBtn->setMinimumSize( 32, 32 );

	connect( saveSettingsBtn, SIGNAL(clicked()), this, SLOT(saveSettingsBtnClicked()));

	saveSettingsBtn->setToolTip(tr("Save current instrument track settings in a preset file"));

	basicControlsLayout->addWidget( saveSettingsBtn, 0, 7 );

	label = new QLabel( tr( "SAVE" ), this );
	label->setStyleSheet( labelStyleSheet );
	basicControlsLayout->addWidget( label, 1, 7);
	basicControlsLayout->setAlignment( label, labelAlignment );

	generalSettingsLayout->addLayout( basicControlsLayout );


	m_tabWidget = new TabWidget( "", this, true, true );
	// "-1" :
	// in "TabWidget::addTab", under "Position tab's window", the widget is
	// moved up by 1 pixel
	m_tabWidget->setMinimumHeight( INSTRUMENT_HEIGHT + GRAPHIC_TAB_HEIGHT - 4 - 1 );


	// create tab-widgets
	m_ssView = new InstrumentSoundShapingView( m_track->soundShaping(), m_tabWidget );

	// FUNC tab
	auto instrumentFunctions = new QWidget(m_tabWidget);
	auto instrumentFunctionsLayout = new QVBoxLayout(instrumentFunctions);
	instrumentFunctionsLayout->setContentsMargins(5, 5, 5, 5);
	m_noteStackingView = new InstrumentFunctionNoteStackingView( m_track->noteStacking() );
	m_arpeggioView = new InstrumentFunctionArpeggioView( m_track->arpeggio() );

	instrumentFunctionsLayout->addWidget( m_noteStackingView );
	instrumentFunctionsLayout->addWidget( m_arpeggioView );
	instrumentFunctionsLayout->addStretch();

	// MIDI tab
	m_midiView = new InstrumentMidiIOView( m_track->midiPortInterface(), m_tabWidget );

	// FX tab
	m_effectView = new EffectRackView( m_track->audioPortInterface()->effectsInterface(), m_tabWidget );

	// MISC tab
	m_miscView = new InstrumentMiscView( m_track, m_tabWidget );


	m_tabWidget->addTab( m_ssView, tr( "Envelope, filter & LFO" ), "env_lfo_tab", 1 );
	m_tabWidget->addTab( instrumentFunctions, tr( "Chord stacking & arpeggio" ), "func_tab", 2 );
	m_tabWidget->addTab( m_effectView, tr( "Effects" ), "fx_tab", 3 );
	m_tabWidget->addTab( m_midiView, tr( "MIDI" ), "midi_tab", 4 );
	m_tabWidget->addTab( m_miscView, tr( "Miscellaneous" ), "misc_tab", 5 );
	adjustTabSize(m_ssView);
	adjustTabSize(instrumentFunctions);
	m_effectView->resize(EffectRackView::DEFAULT_WIDTH, INSTRUMENT_HEIGHT - 4 - 1);
	adjustTabSize(m_midiView);
	adjustTabSize(m_miscView);

	// setup piano-widget
	m_pianoView = new PianoView( m_track->piano(), this );
	m_pianoView->setMinimumHeight( PIANO_HEIGHT );
	m_pianoView->setMaximumHeight( PIANO_HEIGHT );

	vlayout->addWidget( generalSettingsWidget );
	// Use QWidgetItem explicitly to make the size hint change on instrument changes
	// QLayout::addWidget() uses QWidgetItemV2 with size hint caching
	vlayout->insertItem(1, new QWidgetItem(m_tabWidget));
	vlayout->addWidget( m_pianoView );
	
	QObject::connect( m_itv->model()->baseTrack()->model(), SIGNAL(dataChanged()), this, SLOT(update()));
	QObject::connect( m_itv->model()->baseTrack()->model(), SIGNAL(propertiesChanged()), this, SLOT(update()));
	update();

	m_track = m_itv->model();

	m_nameLineEdit->setText( m_track->baseTrack()->name() );

	connect( m_track->baseTrack(), SIGNAL(nameChanged()),
			this, SLOT(updateName()));
	connect( m_track->baseTrack(), SIGNAL(instrumentChanged()),
			this, SLOT(updateInstrumentView()));

	if (m_track->instrument() && m_track->instrument()->flags().testFlag(IInstrument::IsMidiBased))
	{
		m_miscView->microtunerGroupBox()->hide();
		m_track->microtuner()->enabledModel()->setValue(false);
	}
	else
	{
		m_miscView->microtunerGroupBox()->show();
	}

	updateName();



	QMdiSubWindow* subWin = getGUI()->mainWindow()->addWindowedWidget( this );
	Qt::WindowFlags flags = subWin->windowFlags();
	flags |= Qt::MSWindowsFixedSizeDialogHint;
	flags &= ~Qt::WindowMaximizeButtonHint;
	subWin->setWindowFlags( flags );

	updateInstrumentView();

	// Hide the Size and Maximize options from the system menu
	// since the dialog size is fixed.
	QMenu * systemMenu = subWin->systemMenu();
	systemMenu->actions().at( 2 )->setVisible( false ); // Size
	systemMenu->actions().at( 4 )->setVisible( false ); // Maximize

	subWin->setWindowIcon( embed::getIconPixmap( "instrument_track" ) );
	subWin->setMinimumSize( subWin->size() );
	subWin->hide();
}




InstrumentTrackWindow::~InstrumentTrackWindow()
{
	delete m_instrumentView;

	if (parentWidget())
	{
		parentWidget()->hide();
		parentWidget()->deleteLater();
	}
}




void InstrumentTrackWindow::setInstrumentTrackView( InstrumentTrackView* view )
{
	if( m_itv && view )
	{
		m_itv->m_tlb->setChecked( false );
	}

	m_itv = view;
	m_mixerChannelNumber->setTrackView(m_itv);
}




void InstrumentTrackWindow::saveSettingsBtnClicked()
{
	FileDialog sfd(this, tr("Save preset"), "", tr("XML preset file (*.xpf)"));

	QString presetRoot = IConfigManager::Instance()->userPresetsDir();
	if(!QDir(presetRoot).exists())
	{
		QDir().mkdir(presetRoot);
	}
	if(!QDir(presetRoot + m_track->instrumentName()).exists())
	{
		QDir(presetRoot).mkdir(m_track->instrumentName());
	}

	sfd.setAcceptMode(IFileDialog::AcceptSave);
	sfd.setDirectory(presetRoot + m_track->instrumentName());
	sfd.setFileMode( IFileDialog::AnyFile );
	QString fname = m_track->baseTrack()->name();
	sfd.selectFile(fname.remove(QRegExp(FILENAME_FILTER)));
	sfd.setDefaultSuffix( "xpf");

	if( sfd.exec() == QDialog::Accepted &&
		!sfd.selectedFiles().isEmpty() &&
		!sfd.selectedFiles().first().isEmpty() )
	{
		auto dataFile = createDataFile(IDataFile::InstrumentTrackSettings);
		QDomElement& content(dataFile->content());

		m_track->baseTrack()->setSimpleSerializing();
		m_track->saveSettings(*dataFile, content);
		//We don't want to save muted & solo settings when we're saving a preset
		content.setAttribute("muted", 0);
		content.setAttribute("solo", 0);
		content.setAttribute("mutedBeforeSolo", 0);
		QString f = sfd.selectedFiles()[0];
		dataFile->writeFile(f);
	}
}





void InstrumentTrackWindow::updateName()
{
	setWindowTitle( m_track->baseTrack()->name().length() > 25 ? ( m_track->baseTrack()->name().left(24)+"..." ) : m_track->baseTrack()->name() );

	if( m_nameLineEdit->text() != m_track->baseTrack()->name() )
	{
		m_nameLineEdit->setText( m_track->baseTrack()->name() );
	}
}





void InstrumentTrackWindow::updateInstrumentView()
{
	delete m_instrumentView;
	if( m_track->instrument() != nullptr )
	{
		m_instrumentView = static_cast<IQWidgetInstrumentPlugin*>(m_track->instrument()->guiSpecificPlugin())->createView( m_tabWidget );
		m_tabWidget->addTab( m_instrumentView, tr( "Plugin" ), "plugin_tab", 0 );
		m_tabWidget->setActiveTab( 0 );

		m_ssView->setFunctionsHidden( m_track->instrument()->flags().testFlag( IInstrument::IsSingleStreamed ) );

		// TODO: find a way of reapplying this
		// modelChanged(); 		// Get the instrument window to refresh
		m_track->baseTrack()->model()->dataChanged(); // Get the text on the trackButton to change

		adjustTabSize(m_instrumentView);
		m_pianoView->setVisible(m_track->instrument()->hasNoteInput());
		// adjust window size
		layout()->invalidate();
		resize(sizeHint());
		if (parentWidget())
		{
			parentWidget()->resize(parentWidget()->sizeHint());
		}
		update();
		m_instrumentView->update();
	}
}




void InstrumentTrackWindow::textChanged( const QString& newName )
{
	m_track->baseTrack()->setName( newName );
	IEngine::Instance()->getSongInterface()->setModified();
}




void InstrumentTrackWindow::toggleVisibility( bool on )
{
	if( on )
	{
		show();
		parentWidget()->show();
		parentWidget()->raise();
	}
	else
	{
		parentWidget()->hide();
	}
}




void InstrumentTrackWindow::closeEvent( QCloseEvent* event )
{
	event->ignore();

	if( getGUI()->mainWindow()->workspace() )
	{
		parentWidget()->hide();
	}
	else
	{
		hide();
	}

	m_itv->m_tlb->setFocus();
	m_itv->m_tlb->setChecked( false );
}




void InstrumentTrackWindow::focusInEvent( QFocusEvent* )
{
	if(m_pianoView->isVisible()) {
		m_pianoView->setFocus();
	}
}




void InstrumentTrackWindow::dragEnterEventGeneric( QDragEnterEvent* event )
{
	StringPairDrag::processDragEnterEvent( event, "instrument,presetfile,pluginpresetfile" );
}




void InstrumentTrackWindow::dragEnterEvent( QDragEnterEvent* event )
{
	dragEnterEventGeneric( event );
}




void InstrumentTrackWindow::dropEvent( QDropEvent* event )
{
	QString type = StringPairDrag::decodeKey( event );
	QString value = StringPairDrag::decodeValue( event );

	if( type == "instrument" )
	{
		m_track->loadInstrument( value, nullptr, true /* DnD */ );

		IEngine::Instance()->getSongInterface()->setModified();

		event->accept();
		setFocus();
	}
	else if( type == "presetfile" )
	{
		auto dataFile = createDataFile(value);
		m_track->replaceInstrument(*dataFile);
		event->accept();
		setFocus();
	}
	else if( type == "pluginpresetfile" )
	{
		const QString ext = FileItem::extension( value );
		auto* i = m_track->instrument();

		if( !i->descriptor()->supportsFileType( ext ) )
		{
			auto piakn = IPluginFactory::Instance()->pluginSupportingExtension(ext);
			i = m_track->loadInstrument(piakn.name, &piakn.key);
		}

		i->loadFile( value );

		event->accept();
		setFocus();
	}
}




void InstrumentTrackWindow::saveSettings( QDomDocument& doc, QDomElement & thisElement )
{
	thisElement.setAttribute( "tab", m_tabWidget->activeTab() );
	MainWindow::saveWidgetState( this, thisElement );
}




void InstrumentTrackWindow::loadSettings( const QDomElement& thisElement )
{
	m_tabWidget->setActiveTab( thisElement.attribute( "tab" ).toInt() );
	MainWindow::restoreWidgetState( this, thisElement );
	if( isVisible() )
	{
		m_itv->m_tlb->setChecked( true );
	}
}

void InstrumentTrackWindow::viewInstrumentInDirection(int d)
{
	// helper routine for viewNextInstrument, viewPrevInstrument
	// d=-1 to view the previous instrument,
	// d=+1 to view the next instrument

	const QList<TrackView *> &trackViews = m_itv->trackContainerView()->trackViews();
	int idxOfMe = trackViews.indexOf(m_itv);

	// search for the next InstrumentTrackView (i.e. skip AutomationViews, etc)
	// sometimes, the next InstrumentTrackView may already be open, in which case
	//   replace our window contents with the *next* closed Instrument Track and
	//   give focus to the InstrumentTrackView we skipped.
	int idxOfNext = idxOfMe;
	InstrumentTrackView *newView = nullptr;
	InstrumentTrackView *bringToFront = nullptr;
	do
	{
		idxOfNext = (idxOfNext + d + trackViews.size()) % trackViews.size();
		newView = dynamic_cast<InstrumentTrackView*>(trackViews[idxOfNext]);
		// the window that should be brought to focus is the FIRST InstrumentTrackView that comes after us
		if (bringToFront == nullptr && newView != nullptr)
		{
			bringToFront = newView;
		}
		// if the next instrument doesn't have an active window, then exit loop & load that one into our window.
		if (newView != nullptr && !newView->m_tlb->isChecked())
		{
			break;
		}
	} while (idxOfNext != idxOfMe);

	// avoid reloading the window if there is only one instrument, as that will just change the active tab
	if (idxOfNext != idxOfMe)
	{
		// save current window pos and then hide the window by unchecking its button in the track list
		QPoint curPos = parentWidget()->pos();
		m_itv->m_tlb->setChecked(false);

		// enable the new window by checking its track list button & moving it to where our window just was
		newView->m_tlb->setChecked(true);
		newView->getInstrumentTrackWindow()->parentWidget()->move(curPos);

		// scroll the SongEditor/PatternEditor to make sure the new trackview label is visible
		bringToFront->trackContainerView()->scrollToTrackView(bringToFront);

		// TODO: Find a way of reimplementing this!
		// get the instrument window to refresh
		// modelChanged();
	}
	Q_ASSERT(bringToFront);
	bringToFront->getInstrumentTrackWindow()->setFocus();
}

void InstrumentTrackWindow::viewNextInstrument()
{
	viewInstrumentInDirection(+1);
}
void InstrumentTrackWindow::viewPrevInstrument()
{
	viewInstrumentInDirection(-1);
}

void InstrumentTrackWindow::adjustTabSize(QWidget *w)
{
	// "-1" :
	// in "TabWidget::addTab", under "Position tab's window", the widget is
	// moved up by 1 pixel
	w->setMinimumSize(INSTRUMENT_WIDTH - 4, INSTRUMENT_HEIGHT - 4 - 1);
}


} // namespace lmms::gui
