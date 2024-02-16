/*
 * AudioFileProcessor.h - declaration of class AudioFileProcessor
 *                          (instrument-plugin for using audio-files)
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


#ifndef AUDIO_FILE_PROCESSOR_H
#define AUDIO_FILE_PROCESSOR_H


#include "ISampleBuffer.h"
#include "IModels.h"
#include "Instrument.h"

namespace lmms
{

class AudioFileProcessor : public Instrument
{
	Q_OBJECT
public:
	AudioFileProcessor( IInstrumentTrack * _instrument_track );

	void playNote( INotePlayHandle * _n,
						sampleFrame * _working_buffer ) override;
	void deleteNotePluginData( INotePlayHandle * _n ) override;

	void saveSettings( QDomDocument & _doc,
						QDomElement & _parent ) override;
	void loadSettings( const QDomElement & _this ) override;

	void loadFile( const QString & _file ) override;

	QString nodeName() const override;

	virtual int getBeatLen( INotePlayHandle * _n ) const;

	f_cnt_t desiredReleaseFrames() const override
	{
		return 128;
	}


public slots:
	void setAudioFile( const QString & _audio_file, bool _rename = true );


private slots:
	void reverseModelChanged();
	void ampModelChanged();
	void loopPointChanged();
	void startPointChanged();
	void endPointChanged();
	void pointChanged();
	void stutterModelChanged();


signals:
	void isPlaying( lmms::f_cnt_t _current_frame );


private:
	std::unique_ptr<ISampleBuffer> m_sampleBuffer;

	IFloatAutomatableModel* m_ampModel;
	IFloatAutomatableModel* m_startPointModel;
	IFloatAutomatableModel* m_endPointModel;
	IFloatAutomatableModel* m_loopPointModel;
	IBoolAutomatableModel* m_reverseModel;
	IIntAutomatableModel* m_loopModel;
	IBoolAutomatableModel* m_stutterModel;
	IComboBoxModelWrapper* m_interpolationModel;

	f_cnt_t m_nextPlayStartPoint;
	bool m_nextPlayBackwards;
} ;

} // namespace lmms

#endif
