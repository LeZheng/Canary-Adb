#include "cdeviceform.h"
#include "ui_cdeviceform.h"

#include <QLineEdit>//TODO delete

CDeviceForm::CDeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceForm)
{
    ui->setupUi(this);

    ui->refreshToolButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    connect(ui->refreshToolButton,&QToolButton::clicked,[this](){
        this->updateDevices(CAndroidContext::getDevices());
    });

    connect(ui->deviceStackedWidget,&QStackedWidget::customContextMenuRequested,[this](const QPoint &pos) {
        QString allName = this->ui->deviceComboBox->currentText();
        if(!allName.isEmpty()){
            QString tempName = allName.right(allName.size() - allName.indexOf('[') - 1);
            emit itemMenuRequested(tempName.left(tempName.size() - 1));
        }
    });
    connect(ui->deviceComboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),ui->deviceStackedWidget,&QStackedWidget::setCurrentIndex);
}

CDeviceForm::~CDeviceForm()
{
    delete ui;
}

void CDeviceForm::updateDevices(QList<CAndroidDevice *> deviceList)
{
    ui->deviceComboBox->clear();

    if(!deviceList.isEmpty()) {
        QStringList deviceNameList;
        for(int i = 0; i < deviceList.size(); i++) {
            CAndroidDevice * device = deviceList.at(i);
            deviceNameList << tr("%1 [%2]").arg(device->getModel()).arg(device->serialNumber);
//            ui->deviceStackedWidget->addWidget(new QLineEdit(device->getAndroidVersion(),ui->deviceStackedWidget));
        }
        ui->deviceComboBox->addItems(deviceNameList);
    }
}
