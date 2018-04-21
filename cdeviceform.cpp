#include "cdeviceform.h"
#include "ui_cdeviceform.h"

CDeviceForm::CDeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceForm)
{
    ui->setupUi(this);

    connect(ui->deviceListWidget,&QListWidget::doubleClicked,[this](const QModelIndex &index) {
        //TODO
    });

    connect(ui->deviceListWidget,&QListWidget::customContextMenuRequested,[this](const QPoint &pos) {
        //TODO
    });

    connect(ui->deviceListWidget,&QListWidget::clicked,[this](const QModelIndex &index) {
        //TODO
    });
}

CDeviceForm::~CDeviceForm()
{
    delete ui;
}
