/*
 * SampleClipView.cpp
 *
 * Copyright (c) 2005-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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
 
#include "SampleClipView.h"

#include <QApplication>
#include <QMenu>
#include <QPainter>

#include "embed.h"
#include "IPathUtil.h"
#include "ISampleBuffer.h"
#include "IEngine.h"
#include "IClip.h"
#include "ITrack.h"
#include "ISong.h"
#include "StringPairDrag.h"

namespace lmms::gui
{


SampleClipView::SampleClipView( ISampleClip * _clip, TrackView * _tv ) :
	ClipView( _clip->baseClip(), _tv ),
	m_clip( _clip ),
	m_paintPixmap()
{
	// update UI and tooltip
	updateSample();

	// track future changes of ISampleClip
	connect(&m_clip->sampleClipModel(), &SampleClipModel::sampleChanged, this, &SampleClipView::updateSample);

	connect(&m_clip->sampleClipModel(), &SampleClipModel::wasReversed, this, [this](){update();});

	setStyle( QApplication::style() );
}

void SampleClipView::updateSample()
{
	update();
	// set tooltip to filename so that user can see what sample this
	// sample-clip contains
	setToolTip(m_clip->iSampleBuffer()->audioFile() != "" ?
					PathUtil::toAbsolute(m_clip->iSampleBuffer()->audioFile()) :
					tr( "Double-click to open sample" ) );
}




void SampleClipView::constructContextMenu(QMenu* cm)
{
	cm->addSeparator();

	/*contextMenu.addAction( embed::getIconPixmap( "record" ),
				tr( "Set/clear record" ),
						m_clip, SLOT(toggleRecord()));*/

	cm->addAction(
		embed::getIconPixmap("flip_x"),
		tr("Reverse sample"),
		this,
		SLOT(reverseSample())
	);


}




void SampleClipView::dragEnterEvent( QDragEnterEvent * _dee )
{
	if( StringPairDrag::processDragEnterEvent( _dee,
					"samplefile,sampledata" ) == false )
	{
		ClipView::dragEnterEvent( _dee );
	}
}






void SampleClipView::dropEvent( QDropEvent * _de )
{
	if( StringPairDrag::decodeKey( _de ) == "samplefile" )
	{
		m_clip->setSampleFile( StringPairDrag::decodeValue( _de ) );
		_de->accept();
	}
	else if( StringPairDrag::decodeKey( _de ) == "sampledata" )
	{
		m_clip->iSampleBuffer()->loadFromBase64(
					StringPairDrag::decodeValue( _de ) );
		m_clip->updateLength();
		update();
		_de->accept();
		IEngine::Instance()->getSongInterface()->setModified();
	}
	else
	{
		ClipView::dropEvent( _de );
	}
}




void SampleClipView::mousePressEvent( QMouseEvent * _me )
{
	if( _me->button() == Qt::LeftButton &&
		_me->modifiers() & Qt::ControlModifier &&
		_me->modifiers() & Qt::ShiftModifier )
	{
		m_clip->toggleRecord();
	}
	else
	{
		if( _me->button() == Qt::MiddleButton && _me->modifiers() == Qt::ControlModifier )
		{
			auto sClip = dynamic_cast<ISampleClip*>(getClip());
			if( sClip )
			{
				sClip->updateTrackClips();
			}
		}
		ClipView::mousePressEvent( _me );
	}
}




void SampleClipView::mouseReleaseEvent(QMouseEvent *_me)
{
	if( _me->button() == Qt::MiddleButton && !_me->modifiers() )
	{
		auto sClip = dynamic_cast<ISampleClip*>(getClip());
		if( sClip )
		{
			sClip->playbackPositionChanged();
		}
	}
	ClipView::mouseReleaseEvent( _me );
}




void SampleClipView::mouseDoubleClickEvent( QMouseEvent * )
{
	QString af = m_clip->iSampleBuffer()->openAudioFile();

	if ( af.isEmpty() ) {} //Don't do anything if no file is loaded
	else if ( af == m_clip->iSampleBuffer()->audioFile() )
	{	//Instead of reloading the existing file, just reset the size
		int length = (int) ( m_clip->iSampleBuffer()->frames() / IEngine::Instance()->getFramesPerTick() );
		m_clip->baseClip()->changeLength(length);
	}
	else
	{	//Otherwise load the new file as ususal
		m_clip->setSampleFile( af );
		IEngine::Instance()->getSongInterface()->setModified();
	}
}




void SampleClipView::paintEvent( QPaintEvent * pe )
{
	QPainter painter( this );

	if( !needsUpdate() )
	{
		painter.drawPixmap( 0, 0, m_paintPixmap );
		return;
	}

	setNeedsUpdate( false );

	if (m_paintPixmap.isNull() || m_paintPixmap.size() != size())
	{
		m_paintPixmap = QPixmap(size());
	}

	QPainter p( &m_paintPixmap );

	bool muted = m_clip->baseClip()->getITrack()->isMuted() || m_clip->baseClip()->isMuted();
	bool selected = isSelected();

	QLinearGradient lingrad(0, 0, 0, height());
	QColor c = painter.background().color();
	if (muted) { c = c.darker(150); }
	if (selected) { c = c.darker(150); }

	lingrad.setColorAt( 1, c.darker( 300 ) );
	lingrad.setColorAt( 0, c );

	// paint a black rectangle under the clip to prevent glitches with transparent backgrounds
	p.fillRect( rect(), QColor( 0, 0, 0 ) );

	if( gradient() )
	{
		p.fillRect( rect(), lingrad );
	}
	else
	{
		p.fillRect( rect(), c );
	}

	auto clipColor = m_clip->baseClip()->hasColor()
			? m_clip->baseClip()->getEffectiveColor()
			: painter.pen().brush().color();

	p.setPen(clipColor);

	if (muted)
	{
		QColor penColor = p.pen().brush().color();
		penColor.setHsv(penColor.hsvHue(), penColor.hsvSaturation() / 4, penColor.value());
		p.setPen(penColor.darker(250));
	}
	if (selected)
	{
		p.setPen(p.pen().brush().color().darker(150));
	}

	const int spacing = BORDER_WIDTH + 1;
	const float ppb = fixedClips() ?
			( parentWidget()->width() - 2 * BORDER_WIDTH )
					/ (float) m_clip->baseClip()->length().getBar() :
								pixelsPerBar();

	float nom = IEngine::Instance()->getSongInterface()->getTimeSigModelInterface()
				.getNumeratorModel().value();
	float den = IEngine::Instance()->getSongInterface()->getTimeSigModelInterface()
				.getDenominatorModel().value();
	float ticksPerBar = DefaultTicksPerBar * nom / den;

	float offset =  m_clip->baseClip()->startTimeOffset() / ticksPerBar * pixelsPerBar();
	QRect r = QRect( offset, spacing,
			qMax( static_cast<int>( m_clip->sampleLength() * ppb / ticksPerBar ), 1 ), rect().bottom() - 2 * spacing );
	m_clip->iSampleBuffer()->visualize( p, r, pe->rect() );

	QString name = PathUtil::cleanName(m_clip->iSampleBuffer()->audioFile());
	paintTextLabel(name, p);

	// disable antialiasing for borders, since its not needed
	p.setRenderHint( QPainter::Antialiasing, false );

	// inner border
	p.setPen( c.lighter( 135 ) );
	p.drawRect( 1, 1, rect().right() - BORDER_WIDTH,
		rect().bottom() - BORDER_WIDTH );

	// outer border
	p.setPen( c.darker( 200 ) );
	p.drawRect( 0, 0, rect().right(), rect().bottom() );

	// draw the 'muted' pixmap only if the clip was manualy muted
	if( m_clip->baseClip()->isMuted() )
	{
		const int spacing = BORDER_WIDTH;
		const int size = 14;
		p.drawPixmap( spacing, height() - ( size + spacing ),
			embed::getIconPixmap( "muted", size, size ) );
	}

	if ( m_marker )
	{
		p.drawLine(m_markerPos, rect().bottom(), m_markerPos, rect().top());
	}
	// recording sample tracks is not possible at the moment

	/* if( m_clip->isRecord() )
	{
		p.setFont( pointSize<7>( p.font() ) );

		p.setPen( textShadowColor() );
		p.drawText( 10, p.fontMetrics().height()+1, "Rec" );
		p.setPen( textColor() );
		p.drawText( 9, p.fontMetrics().height(), "Rec" );

		p.setBrush( QBrush( textColor() ) );
		p.drawEllipse( 4, 5, 4, 4 );
	}*/

	p.end();

	painter.drawPixmap( 0, 0, m_paintPixmap );
}




void SampleClipView::reverseSample()
{
	m_clip->iSampleBuffer()->setReversed(!m_clip->iSampleBuffer()->reversed());
	IEngine::Instance()->getSongInterface()->setModified();
	update();
}




//! Split this Clip.
/*! \param pos the position of the split, relative to the start of the clip */
bool SampleClipView::splitClip( const TimePos pos )
{
	setMarkerEnabled( false );

	const TimePos splitPos = m_initialClipPos + pos;

	//Don't split if we slid off the Clip or if we're on the clip's start/end
	//Cutting at exactly the start/end position would create a zero length
	//clip (bad), and a clip the same length as the original one (pointless).
	if ( splitPos > m_initialClipPos && splitPos < m_initialClipEnd )
	{
		m_clip->baseClip()->getITrack()->addJournalCheckPoint();
		m_clip->baseClip()->getITrack()->saveJournallingState( false );

		auto rightClip = createSampleClip(*m_clip);

		m_clip->baseClip()->changeLength( splitPos - m_initialClipPos );

		rightClip->baseClip()->movePosition( splitPos );
		rightClip->baseClip()->changeLength( m_initialClipEnd - splitPos );
		rightClip->baseClip()->setStartTimeOffset( m_clip->baseClip()->startTimeOffset() - m_clip->baseClip()->length() );

		m_clip->baseClip()->getITrack()->restoreJournallingState();
		return true;
	}
	else { return false; }
}


} // namespace lmms::gui
