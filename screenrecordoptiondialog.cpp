#include "screenrecordoptiondialog.h"
#include "ui_screenrecordoptiondialog.h"

ScreenRecordOptionDialog::ScreenRecordOptionDialog(CAndroidDevice * device,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenRecordOptionDialog)
{
    ui->setupUi(this);

    QSize wmSize = device->getWmSize();
    if(wmSize.height() <= 0){
        wmSize.setHeight(640);
    }
    if(wmSize.width() <= 0){
        wmSize.setWidth(360);
    }

    ui->widthLineEdit->setValidator(new QIntValidator(0, wmSize.width(), this));
    ui->heightLineEdit->setValidator(new QIntValidator(0, wmSize.height(), this));
    ui->widthLineEdit->setText(tr("%1").arg(wmSize.width()));
    ui->heightLineEdit->setText(tr("%1").arg(wmSize.height()));


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
