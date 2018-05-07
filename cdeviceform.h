#ifndef CDEVICEFORM_H
#define CDEVICEFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QPointer>

#include "candroiddevice.h"

namespace Ui
{
class CDeviceForm;
}

class CDeviceForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceForm(CAndroidDevice * device,QWidget *parent = 0);
    ~CDeviceForm();

public slots:
    void updateDevices();

private:
    Ui::CDeviceForm *ui;
    QPointer<CAndroidDevice> devicePointer;

signals:
    void itemMenuRequested(CAndroidDevice *device);
};

#endif // CDEVICEFORM_H
