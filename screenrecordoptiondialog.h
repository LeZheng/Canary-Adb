#ifndef SCREENRECORDOPTIONDIALOG_H
#define SCREENRECORDOPTIONDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include "candroiddevice.h"

namespace Ui {
class ScreenRecordOptionDialog;
}

class ScreenRecordOptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenRecordOptionDialog(CAndroidDevice * device,QWidget *parent = 0);
    ~ScreenRecordOptionDialog();

    int getBitRate();
    QString getSize();

private:
    Ui::ScreenRecordOptionDialog *ui;
};

#endif // SCREENRECORDOPTIONDIALOG_H
