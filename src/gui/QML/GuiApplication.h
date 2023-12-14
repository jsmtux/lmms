/*
 * GuiApplication.h
 *
 * Copyright (c) 2014 Lukas W <lukaswhl/at/gmail.com>
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

#ifndef LMMS_GUI_GUI_APPLICATION_H
#define LMMS_GUI_GUI_APPLICATION_H

#include "IGuiApplication.h"

#include "ICoreApplication.h"
#include "ITrack.h"
#include "IPiano.h"
#include "LMMSProperty.h"

#include <QGuiApplication>
#include <QClipboard>
#include <QQmlEngine>
#include <QQmlProperty>

#include <QDebug>
#include <QAbstractItemModel>

namespace lmms::gui
{

class InstrumentTrackModel : public QObject {
	Q_OBJECT
public:
	InstrumentTrackModel(IInstrumentTrack* _instrumentTrack, QObject* parent) :
		QObject(parent),
		m_instrumentTrack(_instrumentTrack) {}
	Q_INVOKABLE void testPlay() {
		m_instrumentTrack->piano()->handleKeyPress(40);
	}
private:
	IInstrumentTrack* m_instrumentTrack;
	IFloatAutomatableModel* trackVolumeModel{m_instrumentTrack->volumeModel()};
	EXPOSE_LMMS_PROPERTY(float, volume, trackVolumeModel)
};

class BaseTrackModel : public QObject {
	Q_OBJECT
	Q_PROPERTY(TrackType type READ type CONSTANT)
	Q_PROPERTY(QString name READ name NOTIFY nameChanged)
	Q_PROPERTY(InstrumentTrackModel* instrumentProperties READ instrumentTrackModel CONSTANT)
public:
	enum TrackType {
		Sample,
		Instrument,
		Automation,
		Pattern,
		Other
	};
	Q_ENUM(TrackType)
	static void RegisterInQml() {
		qmlRegisterType<BaseTrackModel>("App", 1, 0, "BaseTrackModel");
		qmlRegisterUncreatableType<TrackType>("App", 1, 0, "TrackType", "Enum type TrackType is not creatable");
	}
	BaseTrackModel(ITrack* track, QObject* parent) :
		QObject(parent),
		m_track(track)
	{
		connect(track, &ITrack::nameChanged, this, &BaseTrackModel::nameChanged);
	}

	TrackType type() {
		switch(m_track->type()) {
			case ITrack::InstrumentTrack:
				return Instrument;
			case ITrack::PatternTrack:
				return Pattern;
			case ITrack::SampleTrack:
				return Sample;
			case ITrack::AutomationTrack:
				return Automation;
			default:
				return Other;
		}
	}

	InstrumentTrackModel* instrumentTrackModel() {
		if (!curInstrumentTrackModel)
			curInstrumentTrackModel = new InstrumentTrackModel(dynamic_cast<IInstrumentTrack*>(m_track->getTrackTypeSpecificInterface()), this);
		return curInstrumentTrackModel;
	}

	QString name() {
		return m_track->name();
	}
signals:
	void nameChanged();
	void propertiesChanged();
private:
	ITrack* m_track;
	IBoolAutomatableModel* trackMutedModel{m_track->getMutedModel()};
	EXPOSE_LMMS_PROPERTY(bool, muted, trackMutedModel)
	IBoolAutomatableModel* trackSoloModel{m_track->soloModel()};
	EXPOSE_LMMS_PROPERTY(bool, solo, trackSoloModel)

	InstrumentTrackModel* curInstrumentTrackModel;
};

class InstrumentClipModel : public QObject {
	Q_OBJECT
	IMidiClip* m_midiClip;
};

class PatternClipModel : public QObject {
	Q_OBJECT
	IPatternClip* m_patternClip;
};

class BaseClipModel : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name CONSTANT)
	enum ClipType {
		None,
		Pattern,
		Instrument
	};
	Q_PROPERTY(ClipType type READ type CONSTANT)
public:
	BaseClipModel(IClip* clip)
		: m_clip(clip)
	{
	}

	ClipType type() {
		switch(m_clip->getType()) {
			case lmms::ClipType::Midi:
				return Instrument;
			case lmms::ClipType::Pattern:
				return Pattern;
			default:
				return None;
		}
	}

	static void RegisterInQml() {
		qmlRegisterType<lmms::gui::BaseClipModel>("App", 1, 0, "BaseClipModel");
	}

	bool isContainedIn(int x_index) {
		return m_clip->startPosition().getBar() <= x_index && m_clip->endPosition().nextFullBar() >= x_index;
	}

	QString name() {
		return m_clip->name();
	}
private:
	IClip* m_clip;
};

class TrackListModel : public QAbstractListModel {
		Q_OBJECT
public:
	TrackListModel(ITrackContainer* trackContainer)
	{
		connect(trackContainer, &ITrackContainer::trackAdded, this, &TrackListModel::trackAdded, Qt::QueuedConnection);
	}

	enum Roles {
		TrackRole = Qt::UserRole
	};

	static void RegisterInQml() {
		qmlRegisterType<lmms::gui::TrackListModel>("App", 1, 0, "TrackListModel");
		BaseTrackModel::RegisterInQml();
	}

	QVariant data(const QModelIndex &index, int role) const override
    {
        if (index.isValid())
		{
			switch (role) {
				case Qt::DisplayRole:
				case TrackRole:
					return QVariant::fromValue(m_trackList[index.row()]);
			}
		}

        return QVariant();
    }

    virtual QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
        roles[TrackRole] = "track";
        return roles;
    }


    int rowCount(const QModelIndex &/*parent*/ = QModelIndex()) const override
    {
        return m_trackList.size();
    }

private slots:
	void trackAdded(ITrack* track) {
		auto index = m_trackList.size();
        beginInsertRows(QModelIndex(), index, index);
		auto trackModel = new BaseTrackModel(track, this);
		m_trackList.append(trackModel);
		endInsertRows();
	}
private:
	QList<BaseTrackModel*> m_trackList;
};

class SongTableModel : public QAbstractItemModel {
		Q_OBJECT
public:
	SongTableModel(ISong* song) :
		m_numBars(song->length())
	{
		connect(song->trackContainerInterface(),
			&ITrackContainer::trackAdded, this, &SongTableModel::trackAdded, Qt::QueuedConnection);
		connect(song, &ISong::lengthChanged, this, [this](int bars){
			int difference = bars - m_numBars;
			if (difference > 0) {
				beginInsertColumns(QModelIndex(), bars, m_numBars);
				endInsertColumns();
			} else if (difference < 0) {
				beginRemoveColumns(QModelIndex(), m_numBars, bars);
				endRemoveColumns();
			}
			m_numBars = bars;
		});
	}

	enum Roles {
		ClipRole = Qt::UserRole
	};

	static void RegisterInQml() {
		qmlRegisterType<lmms::gui::SongTableModel>("App", 1, 0, "SongTableModel");
		BaseClipModel::RegisterInQml();
	}

	QVariant data(const QModelIndex &index, int role) const override
    {
        if (index.isValid())
		{
			switch (role) {
				case Qt::DisplayRole:
					break;
				case ClipRole:
				{
					auto clipList = m_trackClips[m_trackClips.keys()[index.row()]];
					for(const auto& clip: clipList) {
						if(clip->isContainedIn(index.column())) {
							return QVariant::fromValue(clip);
						}
					}
					return QVariant();
				}
			}
		}

        return QVariant();
    }

	QModelIndex index(int row, int column, const QModelIndex &parent) const override
	{
		return hasIndex(row, column, parent) ? createIndex(row, column) : QModelIndex();
	}

	QModelIndex parent(const QModelIndex &index) const override
	{
		return QModelIndex();
	}

    virtual QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
        roles[ClipRole] = "trackClip";
        return roles;
    }


    int rowCount(const QModelIndex &/*parent*/ = QModelIndex()) const override
    {
        return m_trackClips.size();
    }


    int columnCount(const QModelIndex &parent) const override
    {
        return parent.isValid() ? 0 : m_numBars;
    }

private slots:
	void trackAdded(ITrack* track) {
		auto index = m_trackClips.size();
        beginInsertRows(QModelIndex(), index, index);
		auto trackModel = new BaseTrackModel(track, this);
		m_trackClips[trackModel] = QList<BaseClipModel*>();
		connect(track, &ITrack::clipAdded, this, [this, trackModel](IClip* clip){clipAdded(trackModel, clip);});
		for (const auto& clip: track->getClips()) {
			clipAdded(trackModel, clip);
		}
		endInsertRows();
	}

	void clipAdded(BaseTrackModel* track, IClip* clip) {
		auto index = m_trackClips.keys().indexOf(track);
		auto startBar = clip->startPosition().getBar();
		auto endBar = clip->endPosition().nextFullBar();
		m_trackClips[track].push_back(new BaseClipModel(clip));
		emit dataChanged(createIndex(index, startBar), createIndex(index, endBar));
	}
private:
	ISong* song;
	int m_numBars;
	QMap<BaseTrackModel*, QList<BaseClipModel*>> m_trackClips;
};

class ProgressModal : public IProgressModal {
public:
	int value()
	{
		return 0;
	}
	void setValue(int _value)
	{ }
	bool wasCanceled()
	{
		return false;
	}
	void updateDescription(QString description)
	{}
};


class MainWindow: public IMainWindow {
public:
	void ShowInfoMessage(QString title, QString description)
	{
		qDebug() << "Info:" << title << ": " << description << "\n";
	}
	void ShowWarnMessage(QString title, QString description)
	{
		qDebug() << "Warn:" << title << ": " << description << "\n";
	}
	void ShowCriticalMessage(QString title, QString description)
	{
		qDebug() << "Critical:" << title << ": " << description << "\n";
	}
	void ShowTextFloatMessage(QString title, QString description, QPixmap image, int _timeout)
	{
		qDebug() << "TextFloat:" << title << ": " << description << "\n";
	}
	void ShowFileNotFoundMessage(QString path)
	{
		qDebug() << "FileNotfound:" << path << "\n";
	}
	void ShowWarnMessageWithPosition(int line, int col, QString description)
	{
		qDebug() << "Warn:" <<  description << "\n";
	}
	IProgressModal* ShowProgressMessage(QString title, int min, int max)
	{
		return new ProgressModal();
	}
};

class MixerView: public IMixerView {
public:
	void refreshDisplay()
	{}
};

class PianoRollWindow : public IPianoRollWindow {
public:
	int quantization() const
	{
		return 0;
	};
	const IMidiClip* currentMidiClip() const
	{
		return nullptr;
	}
	void update()
	{}
};

class AutomationEditor: public IAutomationEditor{
public:
	void updateAfterClipChange()
	{}
};

class FileDialog: public IFileDialog {
public:
	void setDirectory(const QString& directory) override
	{}
	void setNameFilters(const QStringList &filters) override
	{}
	void selectFile(const QString &filename) override
	{}
	int exec() override
	{
		return IFileDialog::Accepted;
	}
	QStringList selectedFiles() const override
	{
		return QStringList();
	}
	void setAcceptMode(IFileDialog::AcceptMode mode) override
	{}
	void setFileMode(IFileDialog::FileMode mode) override
	{}
};

class LMMS_EXPORT GuiApplication: public QObject, public IGuiApplication
{
	Q_OBJECT
public:
	explicit GuiApplication();
	~GuiApplication() override {}

	static GuiApplication* instance();

	IMainWindow* mainWindowInterface() override
	{
		return &m_mainWindow;
	}
	IMixerView* mixerViewInterface() override
	{
		return &m_mixerView;
	}
	IPianoRollWindow* pianoRollInterface() override
	{
		return &m_pianoRoll;
	}
	IAutomationEditor* automationEditorInterface() override
	{
		return &m_automationEditor;
	}
	std::unique_ptr<IFileDialog> createFileDialog(QString title) override
	{
		return std::make_unique<FileDialog>();
	}
	QClipboard& getClipboard() override
	{
		return  *QGuiApplication::clipboard();
	}
	QString applicationDirPath() override
	{
		return "";
	}

public slots:
	void displayStartupProgress(const QString& msg) {
		qDebug() << "Progress:: " << msg  << "\n";
	}

	void clear() override
	{}
	void restoreState(QDomNode& node) override
	{}
	void saveState(IDataFile& dataFile) override
	{}
private:
	MainWindow m_mainWindow;
	MixerView m_mixerView;
	PianoRollWindow m_pianoRoll;
	AutomationEditor m_automationEditor;

	static std::unique_ptr<GuiApplication> s_instance;
};

} // namespace lmms::gui

#endif // LMMS_GUI_GUI_APPLICATION_H
