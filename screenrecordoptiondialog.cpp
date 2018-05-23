#include "screenrecordoptiondialog.h"
#include "ui_screenrecordoptiondialog.h"

ScreenRecordOptionDialog::ScreenRecordOptionDialog(CAndroidDevice * device,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenRecordOptionDialog)
{
    ui->setupUi(this);

    QString wmSize = device->getWmSize();
    wmSize = wmSize.right(wmSize.size() - wmSize.lastIndexOf(':') - 1).trimmed();
    if(wmSize.contains('x')) {
        int width,height;
        int xIndex = wmSize.indexOf('x');
        bool ok;
        width = wmSize.left(xIndex).toInt(&ok);
        if(ok) {
            height = wmSize.right(wmSize.size() - xIndex - 1).toInt(&ok);
        }
        if(ok) {
            ui->widthLineEdit->setValidator(new QIntValidator(0, width, this));
            ui->heightLineEdit->setValidator(new QIntValidator(0, height, this));
            ui->widthLineEdit->setText(tr("%1").arg(width));
            ui->heightLineEdit->setText(tr("%1").arg(height));
        } else {
            ui->widthLineEdit->setValidator(new QIntValidator(0, 720, this));
            ui->heightLineEdit->setValidator(new QIntValidator(0, 1280, this));
            ui->widthLineEdit->setText(tr("%1").arg(0));
            ui->heightLineEdit->setText(tr("%1").arg(0));
        }
    } else {
        ui->widthLineEdit->setValidator(new QIntValidator(0, 720, this));
        ui->heightLineEdit->setValidator(new QIntValidator(0, 1280, this));
        ui->widthLineEdit->setText(tr("%1").arg(0));
        ui->heightLineEdit->setText(tr("%1").arg(0));
    }

    ui->bitRateLineEdit->setValidator(new QIntValidator(0,6,this));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ScreenRecordOptionDialog::~ScreenRecordOptionDialog()
{
    delete ui;
}

int ScreenRecordOptionDialog::getBitRate()
{
    QString bitRateStr = ui->bitRateLineEdit->text();
    bool ok;
    int bitRate = bitRateStr.toInt(&ok);
    if(ok) {
        return bitRate;
    } else {
        return 4;
    }
}

QString ScreenRecordOptionDialog::getSize()
{
    QString width = ui->widthLineEdit->text();
    QString height = ui->heightLineEdit->text();
    return tr("%1x%2").arg(width).arg(height);
}
