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

    connect(this->ui->detailTabWidget,&QTabWidget::tabCloseRequested,this->ui->detailTabWidget,&QTabWidget::tabBarDoubleClicked);
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

    connect(this,&MainWindow::processStart,this,&MainWindow::showLoadingDialog);
    connect(this,&MainWindow::processEnd,this,&MainWindow::hideLoadingDialog);
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
    connect(fileForm,&CFileForm::menuRequested,this,[this](const QString &path) {
        requestContextMenu(QString(),path);
    });
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
                connect(deviceForm,&CDeviceForm::menuRequested,this,&MainWindow::requestContextMenu);
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
    this->loadingDialog->close();
}

void MainWindow::requestContextMenu(const QString &serialNumber, const QString &path)
{
    QMenu menu;
    if(serialNumber.isEmpty() && !path.isEmpty()) {
        QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
        if(!deviceList.isEmpty()) {
            menu.addAction(tr("rename"),[this,path]() {
                QModelIndex index = this->fileForm->getModel()->index(path);
                if(index.isValid())
                    this->fileForm->getTreeView()->edit(index);
            });
            menu.addAction(tr("delete"),[this,path]() {
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
            if(path.endsWith(".apk")) {
                QMenu *installApkMenu = menu.addMenu(tr("install apk to..."));
                for(int i = 0; i < deviceList.size(); i++) {
                    CAndroidDevice * device = deviceList.at(i);
                    installApkMenu->addAction(tr("%1 [%2]").arg(device->getModel()).arg(device->serialNumber),[device,path,this]() {
                        installApk(device->serialNumber,path);
                    });
                }
            }
            QMenu *pushFileMenu = menu.addMenu(tr("push file to..."));
            for(int i = 0; i < deviceList.size(); i++) {
                CAndroidDevice * device = deviceList.at(i);
                pushFileMenu->addAction(tr("%1 [%2]").arg(device->getModel()).arg(device->serialNumber),[device,path,this]() {
                    //TODO
                });
            }
        }
    }

    if(!serialNumber.isEmpty() && path.isEmpty()) {
        menu.addAction(tr("open detail"),[this,serialNumber]() {
            CAndroidDevice * device = CAndroidContext::getDevice(serialNumber);
            if(device != nullptr)
                openDeviceDetailView(device);
        });
        menu.addAction(tr("screen record"),[this,serialNumber]() {
            screenRecord(serialNumber);
        });
        menu.addAction(tr("screen shot"),[this,serialNumber]() {
            screenShot(serialNumber);
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
        menu.addAction(tr("push"),this,[this,serialNumber]() {
            //TODO
        });
        menu.addAction(tr("install"),this,[this,serialNumber]() {
            QString apkFilePath = QFileDialog::getOpenFileName(this, tr("Get Apk File"),
                                  QDir::rootPath(),
                                  tr("Android Application File (*.apk)"));
            if(!apkFilePath.isEmpty()) {
                installApk(serialNumber,apkFilePath);
            }
        });
        menu.addAction(tr("reboot"),[this]() {
            //TODO
        });
        menu.addAction(tr("logcat"),[this]() {
            //TODO
        });
    }

    if(!serialNumber.isEmpty() && !path.isEmpty()) {
        menu.addAction(tr("install"),this,[this,serialNumber,path]() {
            installApk(serialNumber,path);
        });
        menu.addAction(tr("push"),this,[this,serialNumber,path]() {
            //TODO
        });
    }

    if(menu.actions().size() > 0) {
        menu.exec(QCursor::pos());
    }
}

void MainWindow::installApk(const QString &serialNumber, const QString &path)
{
    CAndroidDevice *device = CAndroidContext::getDevice(serialNumber);
    if(device != nullptr) {
        emit processStart(tr("installing..."),tr("install %1 to %2").arg(path).arg(device->getModel()));
        QtConcurrent::run([device,path,this]() {
            device->install(path);
            emit processEnd(0,"");
        });
    } else {
        //TODO
    }
}

void MainWindow::screenShot(const QString &serialNumber)
{
    QString tmpPhonePath = "/sdcard/canary-screen-shot.png";
    QPointer<CAndroidDevice> device = CAndroidContext::getDevice(serialNumber);
    if(!device.isNull()) {
        emit processStart(tr("screen shot..."),tr("screen shot %1").arg(device->getModel()));

        QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
        connect(watcher,&QFutureWatcher<void>::finished,[this,tmpPhonePath,watcher,device]() {
            QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                               QDir(QDir::homePath()).filePath("screen-shot-untitled.png"),
                               tr("Images (*.png)"));
            if(!savePath.isEmpty()) {
                emit processStart(tr("save..."),tr("save image to %1").arg(savePath));
                QtConcurrent::run([=]() {
                    if(!device.isNull())
                        device->pull(tmpPhonePath,savePath);
                    emit processEnd(0,"");
                });
            }
            watcher->deleteLater();
        });
        watcher->setFuture(QtConcurrent::run([this,tmpPhonePath,device]() {
            QString tmpPhonePath = "/sdcard/canary-screen-shot.png";
            if(!device.isNull())
                device->screenShot(tmpPhonePath);
            emit processEnd(0,"");
        }));
    }
}

void MainWindow::screenRecord(const QString &serialNumber)
{
    QPointer<CAndroidDevice> device = CAndroidContext::getDevice(serialNumber);
    if(!device.isNull()) {
        QString tempPath = "/sdcard/canary-screen-record.mp4";
        ScreenRecordOptionDialog dialog(device,this);
        int resultCode = dialog.exec();
        if(resultCode == QDialog::Accepted) {
            int bitRate = dialog.getBitRate() * 1000000;
            QString size = dialog.getSize();
            QProcess * process = device->screenRecord(tempPath,size,bitRate);
            QProgressDialog *dialog = new QProgressDialog("screen record...", "stop record", 0, 180,this);
            QTimer *t = new QTimer(this);
            connect(t, &QTimer::timeout, this, [dialog]() {
                if(dialog->value() < dialog->maximum()) {
                    dialog->setValue(dialog->value() + 1);
                }
            });
            t->start(1000);
            connect(dialog,&QProgressDialog::canceled,this,[=]() {
                process->kill();
            });
            connect(process,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this,[=]() {
                process->deleteLater();
                dialog->hide();
                dialog->deleteLater();
                t->stop();
                t->deleteLater();
                QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                   QDir(QDir::homePath()).filePath("screen-record-untitled.mp4"),
                                   tr("Video (*.mp4)"));
                if(!savePath.isEmpty()) {
                    emit processStart(tr("save..."),tr("save video to %1").arg(savePath));
                    QtConcurrent::run([=]() {
                        if(!device.isNull())
                            device->pull(tempPath,savePath);
                        emit processEnd(0,"");
                    });
                }
            });
            dialog->show();
        }
    }
}
