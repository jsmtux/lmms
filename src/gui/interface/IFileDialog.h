#ifndef I_FILE_DIALOG_H_
#define I_FILE_DIALOG_H_

#include <QString>
#include <QStringList>

namespace lmms::gui {

class IFileDialog {
public:
	enum DialogCode { Accepted = 1, Rejected = 0 };
	enum AcceptMode { AcceptOpen = 0, AcceptSave = 1 };
	enum FileMode { AnyFile, ExistingFile, Directory, ExistingFiles};
	virtual ~IFileDialog()
	{}
	virtual void setDirectory(const QString& directory) = 0;
	virtual void setNameFilters(const QStringList &filters) = 0;
	virtual void selectFile(const QString &filename) = 0;
	virtual int exec() = 0;
	virtual QStringList selectedFiles() const = 0;
	virtual void setAcceptMode(IFileDialog::AcceptMode mode) = 0;
	virtual void setFileMode(IFileDialog::FileMode mode) = 0;
};

}

#endif /* I_FILE_DIALOG_H_ */
