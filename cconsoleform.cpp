#include "cconsoleform.h"
#include "ui_cconsoleform.h"

CConsoleForm::CConsoleForm(CAndroidDevice *device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CConsoleForm)
{
    ui->setupUi(this);
    devicePointer = device;
}

CConsoleForm::~CConsoleForm()
{
    delete ui;
}
