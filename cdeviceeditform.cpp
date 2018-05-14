#include "cdeviceeditform.h"
#include "ui_cdeviceeditform.h"

CDeviceEditForm::CDeviceEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceEditForm)
{
    ui->setupUi(this);

}

CDeviceEditForm::~CDeviceEditForm()
{
    delete ui;
}
