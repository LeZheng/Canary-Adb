#ifndef CCONSOLEFORM_H
#define CCONSOLEFORM_H

#include <QWidget>
#include <QMenu>
#include <QClipboard>
#include <QFileDialog>
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
    QString deviceSerialNumber;
    QObjectCleanupHandler cleaner;
    bool isDeviceConnected;
    bool isRecording;

    QProcess * logProcess;
};

#endif // CCONSOLEFORM_H
