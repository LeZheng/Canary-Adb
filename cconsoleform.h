#ifndef CCONSOLEFORM_H
#define CCONSOLEFORM_H

#include <QWidget>

#include "candroiddevice.h"

namespace Ui
{
class CConsoleForm;
}

class CConsoleForm : public QWidget
{
    Q_OBJECT

public:
    explicit CConsoleForm(CAndroidDevice *device,QWidget *parent = 0);
    ~CConsoleForm();

signals:
    void logConditionUpdated();

private:
    Ui::CConsoleForm *ui;
    QPointer<CAndroidDevice> devicePointer;
    QObjectCleanupHandler cleaner;

    QProcess * logProcess;
};

#endif // CCONSOLEFORM_H
