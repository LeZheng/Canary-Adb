#ifndef CDEVICEFILEDIALOG_H
#define CDEVICEFILEDIALOG_H

#include <QDialog>
#include <QPushButton>
#include "candroiddevice.h"

namespace Ui {
class CDeviceFileDialog;
}

class CDeviceFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CDeviceFileDialog(CAndroidDevice * device,const QString &defaultName = "",QWidget *parent = 0);
    ~CDeviceFileDialog();

    QString getChoosePath();

private:
    void openDir(const QString &path);

    Ui::CDeviceFileDialog *ui;
    QPointer<CAndroidDevice> devicePointer;
    QString deviceSerialNumber;
    QString currentDir = "";
    QString currentFileName;
    QList<CAndroidFile> currentFiles;
    QStack<QString> prevPathStack;
    QStack<QString> nextPathStack;
};

#endif // CDEVICEFILEDIALOG_H
