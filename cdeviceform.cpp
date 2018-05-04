#include "cdeviceform.h"
#include "ui_cdeviceform.h"

CDeviceForm::CDeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceForm)
{
    ui->setupUi(this);

    connect(ui->deviceListWidget,&QListWidget::itemDoubleClicked,[this](QListWidgetItem *item) {
        emit itemDoubleClicked(item->text());
    });

    connect(ui->deviceListWidget,&QListWidget::customContextMenuRequested,[this](const QPoint &pos) {
        QListWidgetItem * item = this->ui->deviceListWidget->itemAt(pos);
        if(item != nullptr){
            emit itemMenuRequested(pos,this->ui->deviceListWidget->itemAt(pos)->text());
        }
    });

    connect(ui->deviceListWidget,&QListWidget::clicked,[this](const QModelIndex &index) {
        //TODO
    });
}

CDeviceForm::~CDeviceForm()
{
    delete ui;
}

void CDeviceForm::updateDevices(QList<CAndroidDevice *> deviceList)
{
    if(!deviceList.isEmpty()) {
        QStringList deviceNameList;
        for(int i = 0;i < deviceList.size();i++){
            deviceNameList << deviceList.at(i)->serialNumber;
        }
        ui->deviceListWidget->clear();
        ui->deviceListWidget->addItems(deviceNameList);
    }else{
        ui->deviceListWidget->clear();
    }
}
