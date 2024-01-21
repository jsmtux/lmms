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


namespace lmms::gui
{

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
	{}
	void ShowWarnMessage(QString title, QString description)
	{}
	void ShowCriticalMessage(QString title, QString description)
	{}
	void ShowTextFloatMessage(QString title, QString description, QPixmap image, int _timeout)
	{}
	void ShowFileNotFoundMessage(QString path)
	{}
	void ShowWarnMessageWithPosition(int line, int col, QString description)
	{}
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

class LMMS_EXPORT GuiApplication: public IGuiApplication
{
public:
	explicit GuiApplication();
	~GuiApplication() override;

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

	static std::unique_ptr<GuiApplication*> s_instance;
};

} // namespace lmms::gui

#endif // LMMS_GUI_GUI_APPLICATION_H
