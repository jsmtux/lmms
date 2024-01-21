/*
 * AutomationClip.h - declaration of class AutomationClip, which contains
 *                       all information about an automation clip
 *
 * Copyright (c) 2008-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2006-2008 Javier Serrano Polo <jasp00/at/users.sourceforge.net>
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

#ifndef LMMS_AUTOMATION_CLIP_H
#define LMMS_AUTOMATION_CLIP_H

#include <QMap>
#include <QPointer>
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
	#include <QRecursiveMutex>
#endif

#include "AutomationNode.h"
#include "Clip.h"


namespace lmms
{

class AutomationTrack;
class TimePos;



class LMMS_EXPORT AutomationClip : public TypedClip<AutomationTrack>, public IAutomationClip
{
	Q_OBJECT
public:
	AutomationClip( AutomationTrack * _auto_track );
	AutomationClip( const AutomationClip & _clip_to_copy );
	~AutomationClip() override = default;

	IClip* baseClip() override {
		return this;
	}

	const IClip* baseClip() const override {
		return this;
	}

	virtual ClipTypeSpecificInterface* getClipTypeSpecificInterface() override {
		return this;
	}

	bool addObject( IAutomatableModelBase * _obj, bool _search_dup = true ) override;

	const IAutomatableModelBase * firstObject() const override;
	const objectVector& objects() const;

	// progression-type stuff
	inline ProgressionTypes progressionType() const override
	{
		return m_progressionType;
	}
	void setProgressionType( ProgressionTypes _new_progression_type ) override;

	inline float getTension() const override
	{
		return m_tension;
	}
	void setTension( QString _new_tension ) override;

	TimePos timeMapLength() const override;
	void updateLength();

	TimePos putValue(
		const TimePos & time,
		const float value,
		const bool quantPos = true,
		const bool ignoreSurroundingPoints = true
	) override;

	TimePos putValues(
		const TimePos & time,
		const float inValue,
		const float outValue,
		const bool quantPos = true,
		const bool ignoreSurroundingPoints = true
	);

	void removeNode(const TimePos & time) override;
	void removeNodes(const int tick0, const int tick1) override;

	void resetNodes(const int tick0, const int tick1) override;

	void recordValue(TimePos time, float value);

	TimePos setDragValue( const TimePos & time,
				const float value,
				const bool quantPos = true,
				const bool controlKey = false ) override;

	void applyDragValue() override;


	bool isDragging() const
	{
		return m_dragging;
	}

	inline const objectVector & getObjects() const override
	{
		return m_objects;
	}

	inline objectVector & getObjects() override
	{
		return m_objects;
	}


	inline const AutomationTimeMap & getTimeMap() const override
	{
		return m_timeMap;
	}

	inline AutomationTimeMap & getTimeMap() override
	{
		return m_timeMap;
	}

	inline float getMin() const override
	{
		return firstObject()->minValueAsFloat();
	}

	inline float getMax() const override
	{
		return firstObject()->maxValueAsFloat();
	}

	inline bool hasAutomation() const
	{
		return m_timeMap.isEmpty() == false;
	}

	float valueAt( const TimePos & _time ) const;
	float *valuesAfter( const TimePos & _time ) const override;

	QString name() const override;

	// settings-management
	void saveSettings( QDomDocument & _doc, QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	static const QString classNodeName() { return "automationclip"; }
	QString nodeName() const override { return classNodeName(); }

	ClipType getType() override
	{
		return ClipType::Automation;
	}


	static bool isAutomated( const IAutomatableModelBase * _m );
	static QVector<AutomationClip *> clipsForModel( const IAutomatableModelBase * _m );
	static AutomationClip * globalAutomationClip( IAutomatableModelBase * _m );
	static void resolveAllIDs();

	bool isRecording() const override { return m_isRecording; }
	void setRecording( const bool b ) override { m_isRecording = b; }

	static int quantization() { return s_quantization; }
	static void setQuantization(int q) { s_quantization = q; }

	void generateTangents() override;

public slots:
	void clear() override;
	void objectDestroyed( lmms::jo_id_t );
	void flipY( int min, int max ) override;
	void flipY() override;
	void flipX( int length = -1 ) override;

private:
	void cleanObjects();
	void generateTangents(AutomationTimeMap::iterator it, int numToGenerate) override;
	float valueAt( AutomationTimeMap::const_iterator v, int offset ) const;

	// Mutex to make methods involving automation clips thread safe
	// Mutable so we can lock it from const objects
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
	mutable QRecursiveMutex m_clipMutex;
#else
	mutable QMutex m_clipMutex;
#endif
	QVector<jo_id_t> m_idsToResolve;
	objectVector m_objects;
	AutomationTimeMap m_timeMap;	// actual values
	AutomationTimeMap m_oldTimeMap;	// old values for storing the values before setDragValue() is called.
	float m_tension;
	bool m_hasAutomation;
	ProgressionTypes m_progressionType;

	bool m_dragging;
	bool m_dragKeepOutValue; // Should we keep the current dragged node's outValue?
	float m_dragOutValue; // The outValue of the dragged node's

	bool m_isRecording;
	float m_lastRecordedValue;

	static int s_quantization;

	static const float DEFAULT_MIN_VALUE;
	static const float DEFAULT_MAX_VALUE;

} ;


} // namespace lmms

#endif // LMMS_AUTOMATION_CLIP_H
