#include "cdeviceform.h"
#include "ui_cdeviceform.h"

CDeviceForm::CDeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceForm)
{
    ui->setupUi(this);
}

CDeviceForm::~CDeviceForm()
{
    delete ui;
}
