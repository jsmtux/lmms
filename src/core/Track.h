/*
 * Track.h - declaration of Track class
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

#ifndef LMMS_TRACK_H
#define LMMS_TRACK_H

#include <QVector>
#include <QColor>

#include "AutomatableModel.h"
#include "lmms_basics.h"
#include "ITrack.h"

namespace lmms
{

class Clip;
class TimePos;
class TrackContainer;


namespace gui
{

class TrackView;
class TrackContainerView;

}


//! Base-class for all tracks
class LMMS_EXPORT Track : public ITrack
{
	Q_OBJECT
	MM_OPERATORS
	mapPropertyFromModel(bool,isSolo,setSolo,m_soloModel);
public:

	Track( TrackTypes type, TrackContainer * tc );
	~Track() override;

	static Track * create( TrackTypes tt, ITrackContainer * tc );
	static Track * create( const QDomElement & element,
							ITrackContainer * tc );
	ITrack * clone() override;


	// pure virtual functions
	TrackTypes type() const override
	{
		return m_type;
	}

	virtual bool play( const TimePos & start, const fpp_t frames,
						const f_cnt_t frameBase, int clipNum = -1 ) override = 0;

	virtual void saveTrackSpecificSettings( QDomDocument & doc,
						QDomElement & parent ) = 0;
	virtual void loadTrackSpecificSettings( const QDomElement & element ) = 0;


	void saveSettings( QDomDocument & doc, QDomElement & element ) override;
	void loadSettings( const QDomElement & element ) override;

	void setSimpleSerializing() override
	{
		m_simpleSerializingMode = true;
	}

	// -------------------------------------------------------
	void deleteClips() override;

	int numOfClips() override;
	IClip * getClip( int clipNum ) override;
	int getClipNum(const Clip* clip );

	const clipVector & getClips() const override
	{
		return m_clips;
	}
	void getClipsInRange( clipVector & clipV, const TimePos & start,
							const TimePos & end ) override;
	void swapPositionOfClips( int clipNum1, int clipNum2 ) override;

	void createClipsForPattern(int pattern) override;

	bool isMuted() const override
	{
		return m_mutedModel.value();
	}

	void insertBar( const TimePos & pos ) override;
	void removeBar( const TimePos & pos ) override;

	bar_t length() const override;


	inline TrackContainer* trackContainer() const
	{
		return m_trackContainer;
	}

	ITrackContainer* trackInterfaceContainer() const override;

	// name-stuff
	virtual const QString & name() const override
	{
		return m_name;
	}

	inline int getHeight() override
	{
		return m_height >= MINIMAL_TRACK_HEIGHT
			? m_height
			: DEFAULT_TRACK_HEIGHT;
	}
	inline void setHeight( int height ) override
	{
		m_height = height;
	}

	void lock() override
	{
		m_processingLock.lock();
	}
	void unlock() override
	{
		m_processingLock.unlock();
	}
	bool tryLock()
	{
		return m_processingLock.tryLock();
	}
	
	QColor color() override
	{
		return m_color;
	}
	bool useColor() override
	{
		return m_hasColor;
	}

	bool isMutedBeforeSolo() const
	{
		return m_mutedBeforeSolo;
	}
	
	IBoolAutomatableModel* getMutedModel() override;

	Model* model() override {
		return &m_model;
	}

	const TrackTypeSpecificInterface* getTrackTypeSpecificInterface() const override {
		return dynamic_cast<const TrackTypeSpecificInterface*>(this);
	}


	TrackTypeSpecificInterface* getTrackTypeSpecificInterface() override {
		return dynamic_cast<TrackTypeSpecificInterface*>(this);
	}

	IBoolAutomatableModel* soloModel() override {
		return &m_soloModel;
	}

public slots:
	virtual void setName( const QString & newName ) override
	{
		m_name = newName;
		m_model.setDisplayName(m_name);
		emit nameChanged();
	}

	void setMuted( const bool val ) override
	{
		m_mutedModel.setValue( (float) val );
	}

	void setMutedBeforeSolo(const bool muted)
	{
		m_mutedBeforeSolo = muted;
	}

	void toggleSolo();

	void setColor(const QColor& c) override;
	void resetColor() override;

private:
	Model m_model;
	TrackContainer* m_trackContainer;
	TrackTypes m_type;
	QString m_name;
	int m_height;

protected:
	void updateSongProperties();
	BoolModel m_mutedModel;

	clipVector m_clips;

private:
	BoolModel m_soloModel;
	bool m_mutedBeforeSolo;

	bool m_simpleSerializingMode;

	QMutex m_processingLock;
	
	QColor m_color;
	bool m_hasColor;

	friend class gui::TrackView;
} ;


} // namespace lmms

#endif // LMMS_TRACK_H
