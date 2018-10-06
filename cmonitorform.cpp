#include "cmonitorform.h"
#include "ui_cmonitorform.h"

CMonitorForm::CMonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMonitorForm)
{
    ui->setupUi(this);
}

CMonitorForm::~CMonitorForm()
{
    delete ui;
}
