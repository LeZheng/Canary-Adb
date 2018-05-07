#include "cdeviceform.h"
#include "ui_cdeviceform.h"

#include <QLineEdit>//TODO delete

CDeviceForm::CDeviceForm(CAndroidDevice * device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceForm)
{
    ui->setupUi(this);
    devicePointer = device;
    updateDevices();

    connect(ui->deviceIconLabel,&QLabel::customContextMenuRequested,[this]() {
        if(!devicePointer.isNull()) {
            emit itemMenuRequested(devicePointer);
        }
    });
}

CDeviceForm::~CDeviceForm()
{
    delete ui;
}

void CDeviceForm::updateDevices()
{
    if(!devicePointer.isNull()) {
        CAndroidDevice * device = devicePointer;
        ui->androidIdLineEdit->setText(device->getAndroidId());
        ui->androidVersionLineEdit->setText(device->getAndroidVersion());
        ui->deviceNameLineEdit->setText(device->getModel());
        ui->wmDensityLineEdit->setText(device->getWmDensity());
        ui->wmSizeLineEdit->setText(device->getWmSize());
        ui->deviceIconLabel->setPixmap(QPixmap(":/img/phone"));
    }
}
