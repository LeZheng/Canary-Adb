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

private:
    Ui::CConsoleForm *ui;
    QPointer<CAndroidDevice> devicePointer;
};

#endif // CCONSOLEFORM_H
