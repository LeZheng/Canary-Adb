#ifndef CDEVICEEDITFORM_H
#define CDEVICEEDITFORM_H

#include <QWidget>

namespace Ui {
class CDeviceEditForm;
}

class CDeviceEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceEditForm(QWidget *parent = 0);
    ~CDeviceEditForm();

private:
    Ui::CDeviceEditForm *ui;
};

#endif // CDEVICEEDITFORM_H
