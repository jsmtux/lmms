/*
 * TempoSyncKnob.cpp - adds bpm to ms conversion for knob class
 *
 * Copyright (c) 2005-2007 Danny McRae <khjklujn/at/yahoo.com>
 * Copyright (c) 2005-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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


#include "TempoSyncKnob.h"

#include "CaptionMenu.h"
#include "embed.h"
#include "IEngine.h"
#include "GuiApplication.h"
#include "MainWindow.h"
#include "MeterDialog.h"
#include "ISong.h"
#include "SubWindow.h"

#include <QMdiArea>

namespace lmms::gui
{



TempoSyncKnob::TempoSyncKnob( knobTypes _knob_num, ITempoSyncKnobModelWrapper* _model, QWidget * _parent,
						const QString & _name ) :
	Knob( _knob_num, _model->wrappedModel(), _parent, _name ),
	m_tempoSyncIcon( embed::getIconPixmap( "tempo_sync" ) ),
	m_tempoSyncDescription( tr( "Tempo Sync" ) ),
	m_custom( nullptr ),
	m_tempoSyncKnobModel(_model)
{
	connect( model()->modelSignals(), SIGNAL(syncModeChanged(lmms::ITempoSyncKnobModelWrapper::TempoSyncMode)),
			this, SLOT(updateDescAndIcon()));
	connect( this, SIGNAL(sliderMoved(float)),
			model()->modelSignals(), SLOT(disableSync()));
	updateDescAndIcon();
}




TempoSyncKnob::~TempoSyncKnob()
{
	if( m_custom )
	{
		delete m_custom->parentWidget();
	}
}



void TempoSyncKnob::contextMenuEvent( QContextMenuEvent * )
{
	mouseReleaseEvent( nullptr );

	CaptionMenu contextMenu( model()->wrappedModel()->model()->displayName(), this );
	addDefaultActions( &contextMenu );
	contextMenu.addSeparator();

	float limit = 60000.0f / ( IEngine::Instance()->getSongInterface()->getTempo() *
							model()->scale() );

	QMenu * syncMenu = contextMenu.addMenu( m_tempoSyncIcon,
						m_tempoSyncDescription );
	if( limit / 8.0f <= model()->wrappedModel()->maxValue() )
	{

	connect( syncMenu, SIGNAL(triggered(QAction*)),
			model()->modelSignals(), SLOT(setTempoSync(QAction*)));
	syncMenu->addAction( embed::getIconPixmap( "note_none" ),
		tr( "No Sync" ) )->setData( (int) ITempoSyncKnobModelWrapper::SyncNone );
	if( limit / 0.125f <= model()->wrappedModel()->maxValue() )
	{
		syncMenu->addAction( embed::getIconPixmap( "note_double_whole" ),
				tr( "Eight beats" ) )->setData(
				(int) ITempoSyncKnobModelWrapper::SyncDoubleWholeNote );
	}
	if( limit / 0.25f <= model()->wrappedModel()->maxValue() )
	{
		syncMenu->addAction( embed::getIconPixmap( "note_whole" ),
					tr( "Whole note" ) )->setData(
					(int) ITempoSyncKnobModelWrapper::SyncWholeNote );
	}
	if( limit / 0.5f <= model()->wrappedModel()->maxValue() )
	{
		syncMenu->addAction( embed::getIconPixmap( "note_half" ),
					tr( "Half note" ) )->setData(
					(int) ITempoSyncKnobModelWrapper::SyncHalfNote );
	}
	if( limit <= model()->wrappedModel()->maxValue() )
	{
		syncMenu->addAction( embed::getIconPixmap( "note_quarter" ),
					tr( "Quarter note" ) )->setData(
				(int) ITempoSyncKnobModelWrapper::SyncQuarterNote );
	}
	if( limit / 2.0f <= model()->wrappedModel()->maxValue() )
	{
		syncMenu->addAction( embed::getIconPixmap( "note_eighth" ),
					tr( "8th note" ) )->setData(
				(int) ITempoSyncKnobModelWrapper::SyncEighthNote );
	}
	if( limit / 4.0f <= model()->wrappedModel()->maxValue() )
	{
		syncMenu->addAction( embed::getIconPixmap( "note_sixteenth" ),
					tr( "16th note" ) )->setData(
				(int) ITempoSyncKnobModelWrapper::SyncSixteenthNote );
	}
	syncMenu->addAction( embed::getIconPixmap( "note_thirtysecond" ),
					tr( "32nd note" ) )->setData(
				(int) ITempoSyncKnobModelWrapper::SyncThirtysecondNote );
	syncMenu->addAction( embed::getIconPixmap( "dont_know" ),
				tr( "Custom..." ),
				this, SLOT(showCustom())
						)->setData(
					(int) ITempoSyncKnobModelWrapper::SyncCustom );
	contextMenu.addSeparator();

	}
	contextMenu.exec( QCursor::pos() );

	delete syncMenu;
}




void TempoSyncKnob::updateDescAndIcon()
{
	if( model()->tempoSyncMode() )
	{
		switch( model()->tempoSyncMode() )
		{
			case ITempoSyncKnobModelWrapper::SyncCustom:
				m_tempoSyncDescription = tr( "Custom " ) +
						"(" +
			QString::number( model()->custom()->getNumeratorModel().value() ) +
						"/" +
			QString::number( model()->custom()->getDenominatorModel().value() ) +
						")";
				break;
			case ITempoSyncKnobModelWrapper::SyncDoubleWholeNote:
				m_tempoSyncDescription = tr(
						"Synced to Eight Beats" );
				break;
			case ITempoSyncKnobModelWrapper::SyncWholeNote:
				m_tempoSyncDescription = tr(
						"Synced to Whole Note" );
				break;
			case ITempoSyncKnobModelWrapper::SyncHalfNote:
				m_tempoSyncDescription = tr(
							"Synced to Half Note" );
				break;
			case ITempoSyncKnobModelWrapper::SyncQuarterNote:
				m_tempoSyncDescription = tr(
						"Synced to Quarter Note" );
				break;
			case ITempoSyncKnobModelWrapper::SyncEighthNote:
				m_tempoSyncDescription = tr(
							"Synced to 8th Note" );
				break;
			case ITempoSyncKnobModelWrapper::SyncSixteenthNote:
				m_tempoSyncDescription = tr(
							"Synced to 16th Note" );
				break;
			case ITempoSyncKnobModelWrapper::SyncThirtysecondNote:
				m_tempoSyncDescription = tr(
							"Synced to 32nd Note" );
				break;
			default: ;
		}
	}
	else
	{
		m_tempoSyncDescription = tr( "Tempo Sync" );
	}
	if( m_custom != nullptr &&
		model()->tempoSyncMode() != ITempoSyncKnobModelWrapper::SyncCustom )
	{
		m_custom->parentWidget()->hide();
	}

	switch( model()->tempoSyncMode() )
	{
		case ITempoSyncKnobModelWrapper::SyncNone:
			m_tempoSyncIcon = embed::getIconPixmap( "tempo_sync" );
			break;
		case ITempoSyncKnobModelWrapper::SyncCustom:
			m_tempoSyncIcon = embed::getIconPixmap( "dont_know" );
			break;
		case ITempoSyncKnobModelWrapper::SyncDoubleWholeNote:
			m_tempoSyncIcon = embed::getIconPixmap( "note_double_whole" );
			break;
		case ITempoSyncKnobModelWrapper::SyncWholeNote:
			m_tempoSyncIcon = embed::getIconPixmap( "note_whole" );
			break;
		case ITempoSyncKnobModelWrapper::SyncHalfNote:
			m_tempoSyncIcon = embed::getIconPixmap( "note_half" );
			break;
		case ITempoSyncKnobModelWrapper::SyncQuarterNote:
			m_tempoSyncIcon = embed::getIconPixmap( "note_quarter" );
			break;
		case ITempoSyncKnobModelWrapper::SyncEighthNote:
			m_tempoSyncIcon = embed::getIconPixmap( "note_eighth" );
			break;
		case ITempoSyncKnobModelWrapper::SyncSixteenthNote:
			m_tempoSyncIcon = embed::getIconPixmap( "note_sixteenth" );
			break;
		case ITempoSyncKnobModelWrapper::SyncThirtysecondNote:
			m_tempoSyncIcon = embed::getIconPixmap( "note_thirtysecond" );
			break;
		default:
			qWarning( "TempoSyncKnob::calculateTempoSyncTime:"
						"invalid TempoSyncMode" );
			break;
	}

	emit syncDescriptionChanged( m_tempoSyncDescription );
	emit syncIconChanged();
}




const QString & TempoSyncKnob::syncDescription()
{
	return m_tempoSyncDescription;
}




void TempoSyncKnob::setSyncDescription( const QString & _new_description )
{
	m_tempoSyncDescription = _new_description;
	emit syncDescriptionChanged( _new_description );
}




const QPixmap & TempoSyncKnob::syncIcon()
{
	return m_tempoSyncIcon;
}




void TempoSyncKnob::setSyncIcon( const QPixmap & _new_icon )
{
	m_tempoSyncIcon = _new_icon;
	emit syncIconChanged();
}




void TempoSyncKnob::showCustom()
{
	if( m_custom == nullptr )
	{
		m_custom = new MeterDialog( model()->custom(), getGUI()->mainWindow()->workspace() );
		QMdiSubWindow * subWindow = getGUI()->mainWindow()->addWindowedWidget( m_custom );
		Qt::WindowFlags flags = subWindow->windowFlags();
		flags &= ~Qt::WindowMaximizeButtonHint;
		subWindow->setWindowFlags( flags );
		subWindow->setFixedSize( subWindow->size() );
		m_custom->setWindowTitle( "Meter" );
	}
	m_custom->parentWidget()->show();
	model()->setTempoSync( ITempoSyncKnobModelWrapper::SyncCustom );
}





} // namespace lmms::gui
