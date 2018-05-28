#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ui->leftDockWidget->setTitleBarWidget(new QWidget(this));
    this->ui->leftDockWidget->titleBarWidget()->setFixedHeight(0);
    this->ui->rightDockWidget->setTitleBarWidget(new QWidget(this));
    this->ui->rightDockWidget->titleBarWidget()->setFixedHeight(0);

    connect(this->ui->detailTabWidget,&QTabWidget::tabBarDoubleClicked,[this](int index) {
        QWidget * widget = this->ui->detailTabWidget->widget(index);
        QString tabText = this->ui->detailTabWidget->tabText(index);
        tabText = tabText.right(tabText.size() - tabText.lastIndexOf(" [") - 2);
        tabText = tabText.left(tabText.size() - 1);
        if(this->deviceTabMap.contains(tabText)) {
            this->deviceTabMap.remove(tabText);
        }
        this->ui->detailTabWidget->removeTab(index);
        widget->deleteLater();
    });

    loadingDialog = new QProgressDialog(this);
    loadingDialog->setRange(0,100);
    loadingDialog->setValue(100);

    loadAnimation = new QPropertyAnimation(loadingDialog, "value");
    loadAnimation->setDuration(5000);
    loadAnimation->setStartValue(0);
    loadAnimation->setEndValue(80);
    loadAnimation->setEasingCurve(QEasingCurve::OutCubic);

    initToolBar();
    initFileWidget();
    initDeviceWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBar()
{
    //TODO
}

void MainWindow::initFileWidget()
{
    fileForm = new CFileForm(this);
    ui->leftDockWidget->setWidget(fileForm);
    connect(fileForm,&CFileForm::processStart,this,&MainWindow::showLoadingDialog);
    connect(fileForm,&CFileForm::processEnd,this,&MainWindow::hideLoadingDialog);
}

void MainWindow::initDeviceWidget()
{
    ui->refreshToolButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    connect(ui->refreshToolButton,&QToolButton::clicked,[this]() {
        //TODO
    });
    connect(ui->deviceComboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),ui->deviceStackedWidget,&QStackedWidget::setCurrentIndex);
    connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListUpdated,this,[this]() {
        QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
        this->ui->deviceComboBox->clear();
        while(this->ui->deviceStackedWidget->count() > 0) {
            QWidget *widget = this->ui->deviceStackedWidget->widget(0);
            this->ui->deviceStackedWidget->removeWidget(widget);
            delete widget;
        }

        if(!deviceList.isEmpty()) {
            QStringList deviceNameList;
            for(int i = 0; i < deviceList.size(); i++) {
                CAndroidDevice * device = deviceList.at(i);
                deviceNameList << tr("%1 [%2]").arg(device->getModel()).arg(device->serialNumber);
                CDeviceForm *deviceForm = new CDeviceForm(device,this->ui->deviceStackedWidget);
                this->ui->deviceStackedWidget->addWidget(deviceForm);
                connect(deviceForm,&CDeviceForm::requestOpenDetail,this,&MainWindow::openDeviceDetailView);
                connect(deviceForm,&CDeviceForm::processStart,this,&MainWindow::showLoadingDialog);
                connect(deviceForm,&CDeviceForm::processEnd,this,&MainWindow::hideLoadingDialog);
            }
            this->ui->deviceComboBox->addItems(deviceNameList);
        }
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    CAndroidContext::getInstance()->stopListenAdb();
}

void MainWindow::openDeviceDetailView(CAndroidDevice *device)
{
    if(this->deviceTabMap.contains(device->serialNumber)) {
        this->ui->detailTabWidget->setCurrentWidget(this->deviceTabMap.value(device->serialNumber));
    } else {
        QTabWidget *widget = new QTabWidget(this->ui->detailTabWidget);
        widget->setTabPosition(QTabWidget::South);
        CDeviceEditForm * editForm = new CDeviceEditForm(device,widget);
        widget->addTab(editForm,tr("detail"));
        widget->addTab(new CConsoleForm(device,widget),tr("log"));
        this->ui->detailTabWidget->addTab(widget,tr("%1 [%2]").arg(device->getModel()).arg(device->serialNumber));
        this->deviceTabMap.insert(device->serialNumber,widget);

        connect(editForm,&CDeviceEditForm::processStart,this,&MainWindow::showLoadingDialog);
        connect(editForm,&CDeviceEditForm::processEnd,this,&MainWindow::hideLoadingDialog);
    }
}

void MainWindow::showLoadingDialog(const QString &title, const QString &content)
{
    this->loadingDialog->setWindowTitle(title);
    this->loadingDialog->setLabelText(content);
    this->loadingDialog->show();
    this->loadAnimation->start();
}

void MainWindow::hideLoadingDialog(int exitCode, const QString &msg)
{
    this->loadAnimation->stop();
    this->loadingDialog->hide();
}
