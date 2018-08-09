#ifndef CCONSOLEFORM_H
#define CCONSOLEFORM_H

#include <QWidget>
#include <QMenu>
#include <QClipboard>
#include <QFileDialog>
#include <QStateMachine>
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
    void deviceConnected();
    void deviceDisconnected();

private slots:
    void on_actioncopy_triggered();
    void on_actionsave_to_file_triggered();

private:
    Ui::CConsoleForm *ui;
    QPointer<CAndroidDevice> devicePointer;
    QString deviceSerialNumber;
    QObjectCleanupHandler cleaner;

    QProcess * logProcess;

    QStateMachine *machine;
    QState *connectState;
    QState *disconnectState;
    QState *recordingState;
    QState *pauseState;
};

#endif // CCONSOLEFORM_H
