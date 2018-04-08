#ifndef CDEVICEFORM_H
#define CDEVICEFORM_H

#include <QWidget>

namespace Ui {
class CDeviceForm;
}

class CDeviceForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceForm(QWidget *parent = 0);
    ~CDeviceForm();

private:
    Ui::CDeviceForm *ui;
};

#endif // CDEVICEFORM_H
