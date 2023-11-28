/*
 * Instrument.h - declaration of class Instrument, which provides a
 *                standard interface for all instrument plugins
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

#ifndef LMMS_INSTRUMENT_H
#define LMMS_INSTRUMENT_H

#include <QString>
#include <QObject>

#include "lmms_export.h"
#include "lmms_basics.h"
#include "IMemoryManager.h"
#include "Plugin.h"
#include "TimePos.h"
#include "ITrack.h"

namespace lmms
{

// forward-declarations
class NotePlayHandle;
class Track;


class LMMS_EXPORT Instrument : public QObject, public Plugin, public IInstrument
{
	Q_OBJECT
	I_MM_OPERATORS
public:
	Instrument(IInstrumentTrack * _instrument_track,
			const PluginDescriptor * _descriptor,
			const PluginDescriptor::Key * key = nullptr);
	~Instrument() override = default;

	void moveToThread(QThread& thread) override {
		QObject *parent_object = parent();
		setParent( 0 );
		QObject::moveToThread( &thread );
		setParent( parent_object );
	}

	// --------------------------------------------------------------------
	// functions that can/should be re-implemented:

	// --------------------------------------------------------------------

	const SerializingObject* serializingObject() override {
		return this;
	}

	virtual bool hasNoteInput() const override { return true; }

	// if the plugin doesn't play each note, it can create an instrument-
	// play-handle and re-implement this method, so that it mixes its
	// output buffer only once per audio engine period
	virtual void play( sampleFrame * _working_buffer ) override;

	// to be implemented by actual plugin
	virtual void playNote( INotePlayHandle * /* _note_to_play */,
					sampleFrame * /* _working_buf */ ) override
	{
	}

	// needed for deleting plugin-specific-data of a note - plugin has to
	// cast void-ptr so that the plugin-data is deleted properly
	// (call of dtor if it's a class etc.)
	virtual void deleteNotePluginData( INotePlayHandle * _note_to_play ) override;

	// Get number of sample-frames that should be used when playing beat
	// (note with unspecified length)
	// Per default this function returns 0. In this case, channel is using
	// the length of the longest envelope (if one active).
	virtual f_cnt_t beatLen( INotePlayHandle * _n ) const override;


	// some instruments need a certain number of release-frames even
	// if no envelope is active - such instruments can re-implement this
	// method for returning how many frames they at least like to have for
	// release
	virtual f_cnt_t desiredReleaseFrames() const override
	{
		return 0;
	}

	virtual Flags flags() const override
	{
		return NoFlags;
	}

	// sub-classes can re-implement this for receiving all incoming
	// MIDI-events
	inline virtual bool handleMidiEvent( const IMidiEvent&, const TimePos& = TimePos(), f_cnt_t offset = 0 ) override
	{
		return true;
	}

	// QString fullDisplayName() const override;

	// --------------------------------------------------------------------
	// provided functions:
	// --------------------------------------------------------------------

	//! instantiate instrument-plugin with given name or return NULL
	//! on failure
	static IInstrument * instantiate(const QString & _plugin_name,
		IInstrumentTrack * _instrument_track,
		const PluginDescriptor::Key* key,
		bool keyFromDnd = false);

	bool supportsFileType(const QString& extension ) const override {
		return descriptor()->supportsFileType(extension);
	}

	const PluginDescriptor * descriptor() const override {
		return Plugin::descriptor();
	}

	virtual bool isFromTrack( const IInstrumentTrack * _track ) const override;

	inline IInstrumentTrack * instrumentTrack() const override
	{
		return m_instrumentTrack;
	}

	QDomElement saveStateAndKey( QDomDocument & _doc, QDomElement & _parent ) override {
		QDomElement ins = saveState( _doc, _parent );
		if(key().isValid()) {
			ins.appendChild( key().saveXML( _doc ) );
		}
		return ins;
	}
	virtual void restoreState( const QDomElement & _this ) override {
		return JournallingObject::restoreState(_this);
	}

	QString name() override {
		return descriptor()->name;
	}

	QString displayName() override
	{
		return (descriptor()->subPluginFeatures && m_key.isValid())
			// get from sub plugin
			? m_key.displayName()
			// get from plugin
			: descriptor()->displayName;
	}

protected:
	// fade in to prevent clicks
	void applyFadeIn(sampleFrame * buf, INotePlayHandle * n);

	// instruments may use this to apply a soft fade out at the end of
	// notes - method does this only if really less or equal
	// desiredReleaseFrames() frames are left
	void applyRelease( sampleFrame * buf, const INotePlayHandle * _n );


private:
	IInstrumentTrack * m_instrumentTrack;
} ;


} // namespace lmms

#endif // LMMS_INSTRUMENT_H
