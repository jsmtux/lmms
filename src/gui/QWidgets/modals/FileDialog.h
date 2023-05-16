/*
 * FileDialog.h - declaration of class FileDialog
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

#ifndef LMMS_GUI_FILE_DIALOG_H
#define LMMS_GUI_FILE_DIALOG_H

#include "IFileDialog.h"

#include <QFileDialog>

#include "lmms_export.h"

namespace lmms::gui
{


class LMMS_EXPORT FileDialog : public QFileDialog, public IFileDialog
{
	Q_OBJECT
public:
	explicit FileDialog( QWidget *parent = 0, const QString &caption = QString(),
						const QString &directory = QString(),
						const QString &filter = QString() );

	static QString getExistingDirectory(QWidget *parent,
										const QString &caption,
										const QString &directory,
										QFileDialog::Options options = QFileDialog::ShowDirsOnly);
    static QString getOpenFileName(QWidget *parent = 0,
									const QString &caption = QString(),
									const QString &directory = QString(),
									const QString &filter = QString(),
									QString *selectedFilter = 0);
	void clearSelection();
////////////
    void setDirectory(const QString& directory) override {
		QFileDialog::setDirectory(directory);
	}
    void setNameFilters(const QStringList &filters) override {
		QFileDialog::setNameFilters(filters);
	}
    void selectFile(const QString &filename) override {
		QFileDialog::selectFile(filename);
	}
    int exec() override {
		return QFileDialog::exec();
	}
    QStringList selectedFiles() const override {
		return QFileDialog::selectedFiles();
	}

	void setAcceptMode(IFileDialog::AcceptMode mode) override;
	void setFileMode(IFileDialog::FileMode mode) override;
};


} // namespace lmms::gui

#endif // LMMS_GUI_FILE_DIALOG_H
