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

#include "IClip.h"

#include "AutomatableModel.h"

namespace lmms
{

class TrackContainer;

namespace gui
{

class ClipView;
class TrackView;

} // namespace gui

class LMMS_EXPORT Clip : public IClip
{
	Q_OBJECT
	MM_OPERATORS
	mapPropertyFromModel(bool,isSolo,setSolo,m_soloModel);
public:
	Clip( Model * track );
	~Clip() override;

	inline QString name() const override
	{
		return m_name;
	}

	inline void setName( const QString & name ) override
	{
		m_name = name;
		m_clipModel.setDisplayName(name);
		emit m_clipModel.dataChanged();
	}


	inline const TimePos & startPosition() const override
	{
		return m_startPosition;
	}

	inline TimePos endPosition() const override
	{
		const int sp = m_startPosition;
		return sp + m_length;
	}

	const TimePos & length() const override
	{
		return m_length;
	}

	bool isMuted() const override {
		return m_mutedModel.value();
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

	inline const bool getAutoResize() const override
	{
		return m_autoResize;
	}

	QColor color() const override
	{
		return m_color;
	}

	void setColor( const QColor & c ) override
	{
		m_color = c;
	}
	void useCustomClipColor( bool b ) override;

	bool usesCustomClipColor() override
	{
		return m_useCustomClipColor;
	}

	virtual void movePosition( const TimePos & pos ) override;
	virtual void changeLength( const TimePos & length ) override;

	inline void selectViewOnCreate( bool select ) override
	{
		m_selectViewOnCreate = select;
	}

	inline bool getSelectViewOnCreate() override
	{
		return m_selectViewOnCreate;
	}

	/// Returns true if and only if a->startPosition() < b->startPosition()
	static bool comparePosition(const IClip* a, const IClip* b);

	TimePos startTimeOffset() const override;
	void setStartTimeOffset( const TimePos &startTimeOffset ) override;

	// Will copy the state of a clip to another clip
	static void copyStateTo( IClip *src, IClip *dst );

	Model* model() override
	{
		return &m_clipModel;
	}

public slots:
	void toggleMute() override;
	void setMuted(bool _muted) {
		return m_mutedModel.setValue(_muted);
	}


signals:
	void lengthChanged();
	void positionChanged();
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
		Clip(_track->model()),
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

	ITrack* getITrack() const override {
		return m_track;
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
