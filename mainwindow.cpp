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
    this->ui->bottomDockWidget->setTitleBarWidget(new QWidget(this));
    this->ui->bottomDockWidget->titleBarWidget()->setFixedHeight(0);

    initToolBar();
    initFileWidget();
    initDeviceWidget();
    initConsoleWidget();
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
    connect(fileForm,&CFileForm::itemMenuRequested,this,&MainWindow::showFileRequestMenu);
    //TODO
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
        while(this->ui->deviceStackedWidget->count() > 0){
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
                connect(deviceForm,&CDeviceForm::itemMenuRequested,this,&MainWindow::showDeviceRequestMenu);
            }
            this->ui->deviceComboBox->addItems(deviceNameList);
        }

    });
}

void MainWindow::initConsoleWidget()
{
    ui->bottomDockWidget->setWidget(new CConsoleForm(this));
    //TODO
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    CAndroidContext::getInstance()->stopListenAdb();
}

void MainWindow::showFileRequestMenu(const QPoint &pos,const QModelIndex &index,QString path)
{
    QMenu menu;
    menu.addAction(tr("rename"),[this,index]() {
        this->fileForm->getTreeView()->edit(index);
    });
    menu.addAction(tr("delete"),[this,index,path]() {
        QFile::remove(path);
    });
    menu.addAction(tr("open in folder"),[this,path]() {
        QString fileDir = QFileInfo(path).absoluteDir().absolutePath();
        QString urlStr = "file:";
        QDesktopServices::openUrl(QUrl(urlStr.append(fileDir), QUrl::TolerantMode));
    });
    menu.addAction(tr("open default"),[this,path]() {
        QString urlStr = "file:";
        QDesktopServices::openUrl(QUrl(urlStr.append(path), QUrl::TolerantMode));
    });
    if(QFileInfo(path).isDir()) {
        //TODO dir
    } else if(QFileInfo(path).isFile()) {
        //TODO File
        if(path.endsWith(".apk")) {
            QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
            if(!deviceList.isEmpty()) {
                QMenu *installApkMenu = menu.addMenu(tr("install apk to..."));
                for(int i = 0; i < deviceList.size(); i++) {
                    CAndroidDevice * device = deviceList.at(i);
                    installApkMenu->addAction(tr("%1 [%2]").arg(device->getModel()).arg(device->serialNumber),[device,path]() {
                        device->install(path);
                    });
                }
            }
            //TODO apk
        } else {
            //TODO other file
        }
    }
    menu.exec(QCursor::pos());
}

void MainWindow::showDeviceRequestMenu(CAndroidDevice *device)
{
    QMenu menu;
    menu.addAction(tr("record screen"),[this]() {
        //TODO
    });
    menu.addAction(tr("screen shot"),[this]() {
        //TODO
    });
    menu.addAction(tr("set wm size"),[this]() {
        //TODO
    });
    menu.addAction(tr("set wm density"),[this]() {
        //TODO
    });
    menu.addAction(tr("get file"),[this]() {
        //TODO
    });
    menu.addAction(tr("add file"),[this]() {
        //TODO
    });
    menu.addAction(tr("reboot"),[this]() {
        //TODO
    });
    menu.addAction(tr("install app"),[this]() {
        //TODO
    });
    menu.addAction(tr("logcat"),[this]() {
        //TODO
    });
    menu.exec(QCursor::pos());
}
