#include "cfileform.h"
#include "ui_cfileform.h"

CFileForm::CFileForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFileForm)
{
    ui->setupUi(this);
}

CFileForm::~CFileForm()
{
    delete ui;
}
