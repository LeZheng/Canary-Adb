#ifndef CDEVICEFILEFORM_H
#define CDEVICEFILEFORM_H

#include <QWidget>
#include <QTableWidgetItem>
#include "candroiddevice.h"

namespace Ui {
class CDeviceFileForm;
}

class CDeviceFileForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceFileForm(CAndroidDevice * device,QWidget *parent = 0);
    ~CDeviceFileForm();

private:
    void openDir(const QString &path);

    Ui::CDeviceFileForm *ui;
    QPointer<CAndroidDevice> devicePointer;
    QString deviceSerialNumber;
    QString currentDir = "";
    QList<CAndroidFile> currentFiles;
    QStack<QString> prevPathStack;
    QStack<QString> nextPathStack;
};

#endif // CDEVICEFILEFORM_H
