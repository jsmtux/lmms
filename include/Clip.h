/*
 * TrackConteintObject.h - declaration of Clip class
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

#ifndef LMMS_CLIP_H
#define LMMS_CLIP_H

#include <QColor>

#include "AutomatableModel.h"

namespace lmms
{

class TrackContainer;

namespace gui
{

class ClipView;
class TrackView;

} // namespace gui


enum class ClipType
{
	Automation = 0,
	Midi,
	Pattern,
	Sample
};

QString ClipTypeToString(const ClipType _type);
std::size_t ClipTypeToId(const ClipType _type);

class LMMS_EXPORT Clip : public QObject, public JournallingObject
{
	Q_OBJECT
	MM_OPERATORS
	mapPropertyFromModel(bool,isMuted,setMuted,m_mutedModel);
	mapPropertyFromModel(bool,isSolo,setSolo,m_soloModel);
public:
	Clip( Model * track );
	~Clip() override;

	inline const QString & name() const
	{
		return m_name;
	}

	inline void setName( const QString & name )
	{
		m_name = name;
		m_clipModel.setDisplayName(name);
		emit m_clipModel.dataChanged();
	}


	inline const TimePos & startPosition() const
	{
		return m_startPosition;
	}

	inline TimePos endPosition() const
	{
		const int sp = m_startPosition;
		return sp + m_length;
	}

	inline const TimePos & length() const
	{
		return m_length;
	}

	/*! \brief Specify whether or not a TCO automatically resizes.
	 *
	 *  If a TCO does automatically resize, it cannot be manually
	 *  resized by clicking and dragging its edge.
	 *
	 */
	inline void setAutoResize( const bool r )
	{
		m_autoResize = r;
	}

	inline const bool getAutoResize() const
	{
		return m_autoResize;
	}

	QColor color() const
	{
		return m_color;
	}

	void setColor( const QColor & c )
	{
		m_color = c;
	}

	virtual bool hasColor() = 0;

	void useCustomClipColor( bool b );

	bool usesCustomClipColor()
	{
		return m_useCustomClipColor;
	}

	// Return track color unless clip has a custom one
	virtual QColor getEffectiveColor() = 0;

	virtual void movePosition( const TimePos & pos );
	virtual void changeLength( const TimePos & length );

	virtual ClipType getType() = 0;

	inline void selectViewOnCreate( bool select )
	{
		m_selectViewOnCreate = select;
	}

	inline bool getSelectViewOnCreate()
	{
		return m_selectViewOnCreate;
	}

	/// Returns true if and only if a->startPosition() < b->startPosition()
	static bool comparePosition(const Clip* a, const Clip* b);

	TimePos startTimeOffset() const;
	void setStartTimeOffset( const TimePos &startTimeOffset );

	// Will copy the state of a clip to another clip
	static void copyStateTo( Clip *src, Clip *dst );

	Model& model()
	{
		return m_clipModel;
	}

	virtual bool isClipOrTrackMuted() = 0;
	virtual void* getTrackId() const = 0;
	virtual void addJournalCheckPointToTrack() = 0;

public slots:
	void toggleMute();


signals:
	void lengthChanged();
	void positionChanged();
	void destroyedClip();
	void colorChanged();

protected:
	Model m_clipModel;

private:
	enum Actions
	{
		NoAction,
		Move,
		Resize
	} ;

	QString m_name;

	TimePos m_startPosition;
	TimePos m_length;
	TimePos m_startTimeOffset;

	BoolModel m_mutedModel;
	BoolModel m_soloModel;
	bool m_autoResize;

	bool m_selectViewOnCreate;

	QColor m_color;
	bool m_useCustomClipColor;

	friend class ClipView;

} ;


template<class TrackType>
class TypedClip: public Clip
{
public:
	TypedClip( TrackType * _track ) :
		Clip(_track),
		m_track(_track)
	{
		if( m_track )
		{
			m_track->addClip( this );
		}
	}

	virtual ~TypedClip()
	{
		if( m_track )
		{
			m_track->removeClip( this );
		}
	}

	TrackType* getTrack() const {
		return m_track;
	}
	void* getTrackId() const override {
		return m_track;
	}

	void addJournalCheckPointToTrack() override {
		m_track->addJournalCheckPoint();
	}

	virtual bool hasColor() override {
		return usesCustomClipColor() || m_track->useColor();
	}

	bool isClipOrTrackMuted() override {
		return isMuted() || m_track->isMuted();
	}

	QColor getEffectiveColor() override {
		return  usesCustomClipColor()
					? color()
					: getTrack()->color();
	}

private:
	TrackType* m_track;
};


} // namespace lmms

#endif // LMMS_CLIP_H
