#ifndef CDEVICEFORM_H
#define CDEVICEFORM_H

#include <QWidget>

#include "candroiddevice.h"

namespace Ui
{
class CDeviceForm;
}

class CDeviceForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceForm(QWidget *parent = 0);
    ~CDeviceForm();

public slots:
    void updateDevices(QList<CAndroidDevice *> deviceList);

private:
    Ui::CDeviceForm *ui;

signals:
    void itemDoubleClicked(QString name);
    void itemMenuRequested(QString name);
};

#endif // CDEVICEFORM_H
