/*
 * PatternEditor.cpp - basic main-window for editing patterns
 *
 * Copyright (c) 2004-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include "PatternEditor.h"

#include <QAction>

#include "IDataFile.h"
#include "embed.h"
#include "MainWindow.h"
#include "IClip.h"
#include "IPatternStore.h"
#include "ITrack.h"
#include "ISong.h"
#include "IEngine.h"
#include "StringPairDrag.h"

#include "clips/ClipView.h"
#include "tracks/TrackView.h"
#include "widgets/ComboBox.h"


namespace lmms::gui
{


PatternEditor::PatternEditor(IPatternStore* ps) :
	TrackContainerView(&ps->trackContainer()),
	m_ps(ps)
{
}




void PatternEditor::addSteps()
{
	makeSteps( false );
}

void PatternEditor::cloneSteps()
{
	makeSteps( true );
}




void PatternEditor::removeSteps()
{
	TrackList tl = model()->tracks();

	for (const auto& track : tl)
	{
		if (track->type() == ITrack::InstrumentTrack)
		{
			auto p = static_cast<IMidiClip*>(
				track->getClip(m_ps->currentPattern())
					->getClipTypeSpecificInterface());
			p->removeSteps();
		}
	}
}




void PatternEditor::addSampleTrack()
{
	createSampleTrack( model() );
}




void PatternEditor::addAutomationTrack()
{
	createAutomationTrack( model() );
}




void PatternEditor::removeViewsForPattern(int pattern)
{
	for( TrackView* view : trackViews() )
	{
		view->getTrackContentWidget()->removeClipView(pattern);
	}
}



void PatternEditor::saveSettings(QDomDocument& doc, QDomElement& element)
{
	MainWindow::saveWidgetState( parentWidget(), element );
}

void PatternEditor::loadSettings(const QDomElement& element)
{
	MainWindow::restoreWidgetState(parentWidget(), element);
}




void PatternEditor::dropEvent(QDropEvent* de)
{
	QString type = StringPairDrag::decodeKey( de );
	QString value = StringPairDrag::decodeValue( de );

	if( type.left( 6 ) == "track_" )
	{
		auto dataFile = createDataFile( value.toUtf8() );
		ITrack * t = createTrack( dataFile->content().firstChild().toElement(), model() );

		// Ensure pattern clips exist
		bool hasValidPatternClips = false;
		if (t->getClips().size() == IEngine::Instance()->getSongInterface()->numOfPatterns())
		{
			hasValidPatternClips = true;
			for (int i = 0; i < t->getClips().size(); ++i)
			{
				if (t->getClips()[i]->startPosition() != TimePos(i, 0))
				{
					hasValidPatternClips = false;
					break;
				}
			}
		}
		if (!hasValidPatternClips)
		{
			t->deleteClips();
			t->createClipsForPattern(IEngine::Instance()->getSongInterface()->numOfPatterns() - 1);
		}
		IEngine::Instance()->getSongInterface()->setUpPatternStoreTrack();

		de->accept();
	}
	else
	{
		TrackContainerView::dropEvent( de );
	}
}




void PatternEditor::updatePosition()
{
	//realignTracks();
	emit positionChanged( m_currentPosition );
}




void PatternEditor::makeSteps( bool clone )
{
	TrackList tl = model()->tracks();

	for (const auto& track : tl)
	{
		if (track->type() == ITrack::InstrumentTrack)
		{
			auto p = static_cast<IMidiClip*>(track->getClip(m_ps->currentPattern())->getClipTypeSpecificInterface());
			if( clone )
			{
				p->cloneSteps();
			} else
			{
				p->addSteps();
			}
		}
	}
}

// Creates a clone of the current pattern track with the same content, but no clips in the song editor
// TODO: Avoid repeated code from cloneTrack and clearTrack in TrackOperationsWidget somehow
void PatternEditor::cloneClip()
{
	// Get the current IPatternTrack id
	const int currentPattern = m_ps->currentPattern();

	IPatternTrack* pt = IPatternTrack::findPatternTrack(currentPattern);

	if (pt)
	{
		// Clone the track
		ITrack* newTrack = pt->baseTrack()->clone();
		m_ps->setCurrentPattern(static_cast<IPatternTrack*>(
			newTrack->getTrackTypeSpecificInterface())->patternIndex());

		// Track still have the clips which is undesirable in this case, clear the track
		newTrack->lock();
		newTrack->deleteClips();
		newTrack->unlock();
	}
}




PatternEditorWindow::PatternEditorWindow(IPatternStore* ps) :
	Editor(false),
	m_editor(new PatternEditor(ps))
{
	setWindowIcon(embed::getIconPixmap("pattern_track_btn"));
	setWindowTitle(tr("Pattern Editor"));
	setCentralWidget(m_editor);

	setAcceptDrops(true);
	m_toolBar->setAcceptDrops(true);
	connect(m_toolBar, SIGNAL(dragEntered(QDragEnterEvent*)), m_editor, SLOT(dragEnterEvent(QDragEnterEvent*)));
	connect(m_toolBar, SIGNAL(dropped(QDropEvent*)), m_editor, SLOT(dropEvent(QDropEvent*)));

	// TODO: Use style sheet
	if (IConfigManager::Instance()->value("ui", "compacttrackbuttons").toInt())
	{
		setMinimumWidth(TRACK_OP_WIDTH_COMPACT + DEFAULT_SETTINGS_WIDGET_WIDTH_COMPACT + 2 * ClipView::BORDER_WIDTH + 384);
	}
	else
	{
		setMinimumWidth(TRACK_OP_WIDTH + DEFAULT_SETTINGS_WIDGET_WIDTH + 2 * ClipView::BORDER_WIDTH + 384);
	}

	m_playAction->setToolTip(tr("Play/pause current pattern (Space)"));
	m_stopAction->setToolTip(tr("Stop playback of current pattern (Space)"));


	// Pattern selector
	DropToolBar* patternSelectionToolBar = addDropToolBarToTop(tr("Pattern selector"));

	m_patternComboBox = new ComboBox(ps->patternComboboxModel(), m_toolBar);
	m_patternComboBox->setFixedSize(200, ComboBox::DEFAULT_HEIGHT);

	patternSelectionToolBar->addWidget(m_patternComboBox);


	// Track actions
	DropToolBar *trackAndStepActionsToolBar = addDropToolBarToTop(tr("Track and step actions"));


	trackAndStepActionsToolBar->addAction(embed::getIconPixmap("add_pattern_track"), tr("New pattern"),
						IEngine::Instance()->getSongInterface(), SLOT(addPatternTrack()));
	trackAndStepActionsToolBar->addAction(embed::getIconPixmap("clone_pattern_track_clip"), tr("Clone pattern"),
						m_editor, SLOT(cloneClip()));
	trackAndStepActionsToolBar->addAction(embed::getIconPixmap("add_sample_track"),	tr("Add sample-track"),
						m_editor, SLOT(addSampleTrack()));
	trackAndStepActionsToolBar->addAction(embed::getIconPixmap("add_automation"), tr("Add automation-track"),
						m_editor, SLOT(addAutomationTrack()));

	auto stretch = new QWidget(m_toolBar);
	stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	trackAndStepActionsToolBar->addWidget(stretch);


	// Step actions
	trackAndStepActionsToolBar->addAction(embed::getIconPixmap("step_btn_remove"), tr("Remove steps"),
						m_editor, SLOT(removeSteps()));
	trackAndStepActionsToolBar->addAction(embed::getIconPixmap("step_btn_add"), tr("Add steps"),
						m_editor, SLOT(addSteps()));
	trackAndStepActionsToolBar->addAction( embed::getIconPixmap("step_btn_duplicate"), tr("Clone Steps"),
						m_editor, SLOT(cloneSteps()));

	connect(ps->patternComboboxModel()->wrappedModel()->model(), &Model::dataChanged,
			m_editor, &PatternEditor::updatePosition);

	auto viewNext = new QAction(this);
	connect(viewNext, SIGNAL(triggered()), m_patternComboBox, SLOT(selectNext()));
	viewNext->setShortcut(Qt::Key_Plus);
	addAction(viewNext);

	auto viewPrevious = new QAction(this);
	connect(viewPrevious, SIGNAL(triggered()), m_patternComboBox, SLOT(selectPrevious()));
	viewPrevious->setShortcut(Qt::Key_Minus);
	addAction(viewPrevious);
}


QSize PatternEditorWindow::sizeHint() const
{
	return {minimumWidth() + 10, 300};
}


void PatternEditorWindow::play()
{
	if (IEngine::Instance()->getSongInterface()->playMode() != ISong::Mode_PlayPattern)
	{
		IEngine::Instance()->getSongInterface()->playPattern();
	}
	else
	{
		IEngine::Instance()->getSongInterface()->togglePause();
	}
}


void PatternEditorWindow::stop()
{
	IEngine::Instance()->getSongInterface()->stop();
}


} // namespace lmms::gui
