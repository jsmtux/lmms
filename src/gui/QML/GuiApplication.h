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
#include "LMMSProperty.h"

#include <QGuiApplication>
#include <QClipboard>
#include <QQmlEngine>
#include <QQmlProperty>

#include <QDebug>
#include <QAbstractListModel>

namespace lmms::gui
{

class InstrumentTrackModel : public QObject {
	Q_OBJECT
public:
	InstrumentTrackModel(IInstrumentTrack* _instrumentTrack, QObject* parent) :
		QObject(parent),
		m_instrumentTrack(_instrumentTrack) {}
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

class TrackListModel : public QAbstractListModel {
		Q_OBJECT
public:
	TrackListModel(ITrackContainer* trackContainer)
	{
		connect(trackContainer, &ITrackContainer::trackAdded, this, &TrackListModel::trackAdded);
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
        if (!index.isValid())
            return QVariant();

        switch (role) {
			case Qt::DisplayRole:
			case TrackRole:
				return QVariant::fromValue(m_trackList[index.row()]);
        }

        return QVariant();
    }

    virtual QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> tracks;
        tracks[TrackRole] = "track";
        return tracks;
    }


    int rowCount(const QModelIndex &/*parent*/ = QModelIndex()) const override
    {
        return m_trackList.size();
    }

private slots:
	void trackAdded(ITrack* track) {
		auto index = m_trackList.size();
        beginInsertRows(QModelIndex(), index, index);
		m_trackList.append(new BaseTrackModel(track, this));
		endInsertRows();		
	}
private:
	QList<BaseTrackModel*> m_trackList;
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
