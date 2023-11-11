/*
 * Plugin.h - class plugin, the base-class and generic interface for all plugins
 *
 * Copyright (c) 2005-2023 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef LMMS_I_PLUGIN_H
#define LMMS_I_PLUGIN_H

#include <QDomElement>
#include <QString>

#include "IMidiEvent.h"
#include "IPlayHandle.h"
#include "ITrack.h"
#include "PluginDescriptor.h"
#include "lmms_basics.h"

namespace lmms {

class EffectControls;
class SerializingObject;

class IGUISpecificPlugin {};

class IPlugin {
public:
	virtual ~IPlugin() = default;
	IGUISpecificPlugin* guiSpecificPlugin() { return nullptr; }
};

class ITool : public IPlugin
{
};

class IInstrument : public IPlugin
{
public:
	enum Flag
	{
		NoFlags = 0x00,
		IsSingleStreamed = 0x01, /*! Instrument provides a single audio stream for all notes */
		IsMidiBased = 0x02,		 /*! Instrument is controlled by MIDI events rather than NotePlayHandles */
		IsNotBendable = 0x04,	 /*! Instrument can't react to pitch bend changes */
	};

	virtual ~IInstrument() = default;

	Q_DECLARE_FLAGS(Flags, Flag);

	virtual QString name() = 0;

	virtual ITrack* instrumentTrack() const = 0;
	virtual void play(sampleFrame* _working_buffer) = 0;
	virtual bool isFromTrack(const ITrack* _track) const = 0;
	virtual f_cnt_t desiredReleaseFrames() const = 0;
	virtual Flags flags() const = 0;
	virtual bool supportsFileType(const QString& extension) const = 0;
	virtual const PluginDescriptor* descriptor() const = 0;

	//! Can be called if a file matching supportedFileTypes should be
	//! loaded/processed with the help of this plugin
	virtual void loadFile(const QString& file){};

	virtual f_cnt_t beatLen(INotePlayHandle* _n) const = 0;
	virtual void playNote(INotePlayHandle* note_to_play, sampleFrame* _working_buf) = 0;
	virtual void deleteNotePluginData(INotePlayHandle* _note_to_play) = 0;

	virtual const SerializingObject* serializingObject() = 0;
	virtual QDomElement saveStateAndKey(QDomDocument& _doc, QDomElement& _parent) = 0;
	virtual void restoreState(const QDomElement& _this) = 0;

	virtual QString displayName() = 0;

	virtual bool handleMidiEvent(const IMidiEvent&, const TimePos& = TimePos(), f_cnt_t offset = 0) = 0;
	virtual void moveToThread(QThread& thread) = 0;
	virtual bool hasNoteInput() const = 0;
};

IInstrument* InstantiateDummyInstrument(ITrack* _instrument_track);

IInstrument* InstantiateInstrument(
	const QString& pluginName, ITrack* _parent, const PluginDescriptor::Key* _key, bool keyFromDnd = false);

Q_DECLARE_OPERATORS_FOR_FLAGS(IInstrument::Flags)

class IEffect : public QObject, public IPlugin
{
	Q_OBJECT
public:
	virtual ~IEffect() = default;
	virtual bool isDummy() = 0;
	virtual bool isOkay() const = 0;
	virtual void startRunning() = 0;
	virtual bool isRunning() const = 0;
	virtual bool processAudioBuffer(sampleFrame* _buf, const fpp_t _frames) = 0;
	virtual QDomElement saveState(QDomDocument& _doc, QDomElement& _parent) = 0;
	virtual const PluginDescriptor* descriptor() const = 0;
	virtual const PluginDescriptor::Key& key() = 0;
	virtual void restoreState(const QDomElement& _this) = 0;
	virtual QString nodeName() const = 0;
	virtual IAutomatableModel<bool>* enabledModel() = 0;
	virtual IAutomatableModel<float>* wetDryModel() = 0;
	virtual IAutomatableModel<float>* gateModel() = 0;
	virtual ITempoSyncKnobModelWrapper* autoQuitModel() = 0;
	virtual bool autoQuitDisabled() = 0;
	virtual EffectControls* controls() = 0;
	virtual QString displayName() = 0;
	virtual IEffectChain* model() = 0;
};

class Model;
class IEffectChain;
class IDummyEffect{};

IEffect* InstantiateDummyEffect(IEffectChain* _parent, const QDomElement& originalPluginData);

IEffect* InstantiateEffect(const QString& pluginName, IEffectChain* _parent, PluginDescriptor::Key* _key);

class IExportFilter : public IPlugin
{
public:
	virtual bool tryExport(
		const TrackList& tracks, const TrackList& patternTracks, int tempo, int masterPitch, const QString& filename)
		= 0;
};

IExportFilter* InstantiateMidiExportFilter();
void importFilterImport( const QString & _file_to_import, ITrackContainer* tc );

// Alias for RemotePluginBase::isMainThreadWaiting
bool IsRemotePluginMainThreadWaiting();

} // namespace lmms

#endif // LMMS_I_PLUGIN_H