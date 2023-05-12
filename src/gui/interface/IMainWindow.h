#ifndef E270C3D0_0FB7_4E30_984F_A65D1EC26522
#define E270C3D0_0FB7_4E30_984F_A65D1EC26522

#include <QString>
#include <QPixmap>

namespace lmms::gui {

class IProgressModal {
public:
    virtual ~IProgressModal() {}
    virtual int value() = 0;
    virtual void setValue(int value) = 0;
    virtual bool wasCanceled() = 0;
    virtual void updateDescription(QString description) = 0;
};

class IMainWindow {
public:
    virtual ~IMainWindow() {}
    virtual void ShowInfoMessage(QString title, QString description) = 0;
    virtual void ShowCriticalMessage(QString title, QString description) = 0;
    virtual void ShowTextFloatMessage(QString title, QString description, QPixmap image, int _timeout) = 0;
    virtual void ShowFileNotFoundMessage(QString path) = 0;
    virtual void ShowWarningMessage(int line, int col, QString description) = 0;
    virtual IProgressModal* ShowProgressMessage(QString title, int min, int max) = 0;
};

}

#endif /* E270C3D0_0FB7_4E30_984F_A65D1EC26522 */
