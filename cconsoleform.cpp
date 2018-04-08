#include "cconsoleform.h"
#include "ui_cconsoleform.h"

CConsoleForm::CConsoleForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CConsoleForm)
{
    ui->setupUi(this);
}

CConsoleForm::~CConsoleForm()
{
    delete ui;
}
