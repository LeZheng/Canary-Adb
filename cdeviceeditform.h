#ifndef CDEVICEEDITFORM_H
#define CDEVICEEDITFORM_H

#include <QWidget>
#include <QFileDialog>
#include <QFutureWatcher>
#include "candroiddevice.h"

namespace Ui {
class CDeviceEditForm;
}

class CDeviceEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceEditForm(CAndroidDevice * device,QWidget *parent = 0);
    ~CDeviceEditForm();

signals:
    void processStart(const QString &title,const QString &content);
    void processEnd(int exitCode,const QString &msg);

private:
    Ui::CDeviceEditForm *ui;
    QPointer<CAndroidDevice> devicePointer;

    void inputKeyEvent(int keyCode);
};

#endif // CDEVICEEDITFORM_H
