#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    deviceComboBox = new QComboBox(this);

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
    connect(ui->actionscreen_shot,&QAction::triggered,this,[this]() {
        if(this->deviceComboBox->currentIndex() >= 0 && this->deviceComboBox->currentIndex() < this->serialNumberList.size()) {
            screenShot(this->serialNumberList.at(this->deviceComboBox->currentIndex()));
        }
    });
    connect(ui->actionscreen_record,&QAction::triggered,this,[this]() {
        if(this->deviceComboBox->currentIndex() >= 0 && this->deviceComboBox->currentIndex() < this->serialNumberList.size()) {
            screenRecord(this->serialNumberList.at(this->deviceComboBox->currentIndex()));
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBar()
{
    QList<QAction *> actions = ui->leftToolBar->actions();
    for(int i = 0; i < actions.size(); i++) {
        QAction *action = actions.at(i);
        ui->leftToolBar->widgetForAction(action)->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        connect(action,&QAction::triggered,this,[this,action,i](bool checked) {
            foreach (QAction *tempAction, ui->leftToolBar->actions()) {
                tempAction->setChecked(tempAction == action);
            }
            int currentDeviceIndex = this->deviceComboBox->currentIndex();
            if(currentDeviceIndex >= 0) {
                this->ui->deviceStackedWidget->setCurrentIndex(currentDeviceIndex);
                QStackedWidget * stackWidget = qobject_cast<QStackedWidget *>(this->ui->deviceStackedWidget->currentWidget());
                if(stackWidget && i < stackWidget->count()) {
                    stackWidget->setCurrentIndex(i);
                }
            }
        });
    }
    actions.at(0)->setChecked(true);

    QWidget *spaceWidget = new QWidget(ui->mainToolBar);
    spaceWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spaceWidget->setMinimumWidth(ui->leftToolBar->width() - 20);
    ui->mainToolBar->addWidget(spaceWidget);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(deviceComboBox);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionscreen_shot);
    ui->mainToolBar->addAction(ui->actionscreen_record);
}

void MainWindow::initFileWidget()
{
    QToolBar *titleWidget = new QToolBar(this->ui->rightDockWidget);
    QToolButton *addTabButton = new QToolButton(titleWidget);
    addTabButton->setIcon(QIcon(":/img/ic_new"));
    connect(addTabButton,&QToolButton::clicked,this,[this]() {
        QSplitter *splitter = new QSplitter(this->ui->fileTabWidget);
        splitter->setOrientation(Qt::Vertical);
        for(int i = 0; i < 2; i++) {
            CFileForm *fileForm = new CFileForm(splitter);
            if(i == 0) {
                fileForm->setSelect(true);
            }
            connect(fileForm,&CFileForm::menuRequested,this,&MainWindow::requestContextMenu);
            connect(fileForm,&CFileForm::basePathChanged,this,[this,fileForm](const QString &path) {
                QFileInfo file(path);
                int index = this->ui->fileTabWidget->indexOf(fileForm->parentWidget());
                if(index >= 0) {
                    this->ui->fileTabWidget->setTabText(index,file.fileName());
                }
            });
            connect(fileForm,&CFileForm::selected,this,[this,fileForm,splitter]() {
                for(int i = 0; i < splitter->count(); i++) {
                    QWidget *widget = splitter->widget(i);
                    if(widget->inherits("CFileForm")) {
                        if(fileForm != widget) {
                            qobject_cast<CFileForm *>(widget)->setSelect(false);
                        } else {
                            qobject_cast<CFileForm *>(widget)->setSelect(true);
                            emit currentFileFormChanged(qobject_cast<CFileForm *>(widget));
                        }
                    }
                }
            });
            fileForm->setViewMode(FileItemMode::TREE);
        }
        splitter->widget(1)->hide();
        connect(ui->rightDockWidget,&QDockWidget::dockLocationChanged,this,[this,splitter](Qt::DockWidgetArea area) {
            if(area == Qt::RightDockWidgetArea) {
                splitter->setOrientation(Qt::Vertical);
            } else if(area == Qt::BottomDockWidgetArea) {
                splitter->setOrientation(Qt::Horizontal);
            }
        });
        this->ui->fileTabWidget->addTab(splitter,QApplication::style()->standardIcon(QStyle::SP_DirIcon),QDir::rootPath());
        if(this->ui->fileTabWidget->count() > 1) {
            this->ui->fileTabWidget->setTabBarAutoHide(false);
        }
    });
    QToolButton *gridViewButton = new QToolButton(titleWidget);
    gridViewButton->setIcon(QIcon(":/img/ic_grid"));
    gridViewButton->setCheckable(true);
//    QToolButton *listViewButton = new QToolButton(titleWidget);
//    listViewButton->setIcon(QIcon(":/img/view_list"));
//    listViewButton->setCheckable(true);
    QToolButton *treeViewButton = new QToolButton(titleWidget);
    treeViewButton->setIcon(QIcon(":/img/ic_list"));
    treeViewButton->setCheckable(true);
//    connect(listViewButton,&QToolButton::clicked,[=]() {
//        changeFileViewMode(FileItemMode::LIST);
//        emit this->ui->fileTabWidget->currentChanged(this->ui->fileTabWidget->currentIndex());
//    });
    connect(gridViewButton,&QToolButton::clicked,[=]() {
        changeFileViewMode(FileItemMode::GRID);
        emit this->ui->fileTabWidget->currentChanged(this->ui->fileTabWidget->currentIndex());
    });
    connect(treeViewButton,&QToolButton::clicked,[=]() {
        changeFileViewMode(FileItemMode::TREE);
        emit this->ui->fileTabWidget->currentChanged(this->ui->fileTabWidget->currentIndex());
    });

    QToolButton *splitButton = new QToolButton(titleWidget);
    splitButton->setIcon(QIcon(":/img/view_split"));
    splitButton->setText(tr("split"));
    splitButton->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    connect(splitButton,&QToolButton::clicked,[this,splitButton]() {
        QWidget *widget = this->ui->fileTabWidget->currentWidget();
        if(widget->inherits("QSplitter")) {
            QSplitter *splitter = qobject_cast<QSplitter *>(widget);
            if(splitter->widget(0)->isVisible() && splitter->widget(1)->isVisible()) {
                for(int i = 0; i < splitter->count(); i++) {
                    QWidget *fileWidget = splitter->widget(i);
                    if(fileWidget->inherits("CFileForm") && qobject_cast<CFileForm *>(fileWidget)->isSelect()) {
                        splitter->widget(i)->hide();
                        qobject_cast<CFileForm *>(splitter->widget(i % 1))->setSelect(true);
                        splitButton->setIcon(QIcon(":/img/view_split"));
                        splitButton->setText(tr("split"));
                        return;
                    }
                }
            } else {
                for(int i = 0; i < splitter->count(); i++) {
                    if(!splitter->widget(i)->isVisible()) {
                        splitter->widget(i)->show();
                        qobject_cast<CFileForm *>(splitter->widget(i))->setSelect(true);
                    } else {
                        qobject_cast<CFileForm *>(splitter->widget(i))->setSelect(false);
                    }
                }
                splitButton->setText(tr("close"));
                splitButton->setIcon(QIcon(":/img/view_right_close"));
            }
        }
    });

    titleWidget->addWidget(addTabButton);
    titleWidget->addSeparator();
    titleWidget->addWidget(gridViewButton);
//    titleWidget->addWidget(listViewButton);
    titleWidget->addWidget(treeViewButton);
    titleWidget->addSeparator();
    titleWidget->addWidget(splitButton);

    titleWidget->layout()->setMargin(6);
    this->ui->rightDockWidget->setTitleBarWidget(titleWidget);

    connect(ui->fileTabWidget,&QTabWidget::tabBarDoubleClicked,ui->fileTabWidget,&QTabWidget::tabCloseRequested);
    connect(ui->fileTabWidget,&QTabWidget::tabCloseRequested,this,[this](int index) {
        QWidget *widget = this->ui->fileTabWidget->widget(index);
        this->ui->fileTabWidget->removeTab(index);
        widget->deleteLater();
        if(this->ui->fileTabWidget->count() <= 1) {
            this->ui->fileTabWidget->setTabBarAutoHide(true);
        }
    });
    connect(ui->fileTabWidget,&QTabWidget::currentChanged,this,[=](int index) {
        QWidget *widget = this->ui->fileTabWidget->widget(index);
        if(widget->inherits("QSplitter")) {
            QSplitter *splitter = qobject_cast<QSplitter *>(widget);
            for(int i = 0; i < splitter->count(); i++) {
                widget = splitter->widget(i);//TODO
                if(widget->inherits("CFileForm")) {
                    CFileForm *fileForm = qobject_cast<CFileForm *>(widget);
                    if(fileForm->isSelect()) {
                        emit currentFileFormChanged(fileForm);
                    }
                }
            }

        }
    });
    connect(this,&MainWindow::currentFileFormChanged,this,[=](CFileForm *form) {
        FileItemMode mode = form->viewMode();
        gridViewButton->setChecked(mode == FileItemMode::GRID ? true : false);
//        listViewButton->setChecked(mode == FileItemMode::LIST ? true : false);
        treeViewButton->setChecked(mode == FileItemMode::TREE ? true : false);
    });
    this->ui->fileTabWidget->setTabBarAutoHide(true);
    emit addTabButton->clicked(true);
    emit treeViewButton->clicked(true);
}

void MainWindow::initDeviceWidget()
{
    connect(deviceComboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),ui->deviceStackedWidget,&QStackedWidget::setCurrentIndex);
    connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListUpdated,this,[this]() {
        QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
        for(int i = 0; i < deviceList.size(); i++) {
            CAndroidDevice * device = deviceList.at(i);
            const QString serialNumber = device->serialNumber;
            if(!this->deviceStackMap.contains(serialNumber)) {
                const int index = this->deviceComboBox->count();
                this->deviceComboBox->addItem(QString("%1 [%2]").arg(device->getModel()).arg(device->serialNumber));
                this->serialNumberList.insert(index,serialNumber);

                QStackedWidget *stackedWidget = new QStackedWidget(this->ui->deviceStackedWidget);

                CDeviceForm *deviceInfoForm = new CDeviceForm(device,stackedWidget);
                deviceInfoForm->updateDevices();
                connect(deviceInfoForm,&CDeviceForm::menuRequested,this,&MainWindow::requestContextMenu);
                stackedWidget->addWidget(deviceInfoForm);

                QListWidget *appListWidget = new QListWidget(stackedWidget);
                appListWidget->clear();
                foreach (CAndroidApp * app, device->getApplications()) {
                    appListWidget->addItem(app->getName());
                }
                stackedWidget->addWidget(appListWidget);

                CDeviceFileForm *fileForm = new CDeviceFileForm(device,stackedWidget);
                connect(fileForm,&CDeviceFileForm::menuRequested,this,&MainWindow::requestContextMenu);
                stackedWidget->addWidget(fileForm);

                stackedWidget->addWidget(new CConsoleForm(device,stackedWidget));

                CDeviceEditForm *editForm = new CDeviceEditForm(device,stackedWidget);
                connect(editForm,&CDeviceEditForm::requestScreenShot,this,&MainWindow::screenShot);
                connect(editForm,&CDeviceEditForm::requestScreenRecord,this,&MainWindow::screenRecord);
                connect(editForm,&CDeviceEditForm::processStart,this,&MainWindow::showLoadingDialog);
                connect(editForm,&CDeviceEditForm::processEnd,this,&MainWindow::hideLoadingDialog);
                stackedWidget->addWidget(editForm);

                //TODO
                this->ui->deviceStackedWidget->addWidget(stackedWidget);
                this->deviceStackMap.insert(serialNumber,stackedWidget);

                connect(device,&CAndroidDevice::destroyed,this,[this,serialNumber,index]() {
                    this->deviceComboBox->removeItem(index);
                    QStackedWidget *stackedWidget = this->deviceStackMap.take(serialNumber);
                    stackedWidget->deleteLater();
                    if(serialNumber == this->serialNumberList.at(index)) {
                        this->serialNumberList.removeAt(index);
                    } else {
                        this->serialNumberList.removeOne(serialNumber);
                    }
                });
            }
        }
    });
    emit CAndroidContext::getInstance()->deviceListUpdated();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    CAndroidContext::getInstance()->stopListenAdb();
}

void MainWindow::changeFileViewMode(FileItemMode mode)
{
    if(this->ui->fileTabWidget->currentWidget()->inherits("QSplitter")) {
        QSplitter *splitter = qobject_cast<QSplitter *>(this->ui->fileTabWidget->currentWidget());
        for(int i = 0; i < splitter->count(); i++) {
            if(splitter->widget(i)->inherits("CFileForm")) {
                CFileForm *fileForm = qobject_cast<CFileForm *>(splitter->widget(i));
                if(fileForm->isSelect()) {
                    fileForm->setViewMode(mode);
                    return;
                }
            }
        }
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
    if(exitCode != 0 && !msg.isEmpty()) {
        QFontMetrics metrics(QToolTip::font());
        QRect msgRect = metrics.boundingRect(msg);
        QRect rect = this->frameGeometry();
        QToolTip::showText(QPoint(rect.right() - msgRect.width() - 20,rect.bottom() - msgRect.height() - 20),
                           msg,this,this->frameGeometry(),5000);//TODO
    }
}

void MainWindow::requestContextMenu(const QString &serialNumber, const QString &localPath,const QString &devicePath)
{
    QMenu menu;
    if(serialNumber.isEmpty() && !localPath.isEmpty()) {
        QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
        menu.addAction(QIcon(":/img/edit_rename"),tr("rename"),[this,localPath]() {
            if(this->ui->fileTabWidget->currentWidget()->inherits("QSplitter")) {
                QSplitter *splitter = qobject_cast<QSplitter *>(this->ui->fileTabWidget->currentWidget());
                if(splitter->widget(0)->inherits("CFileForm")) {
                    CFileForm *fileForm = qobject_cast<CFileForm *>(splitter->widget(0));
                    QModelIndex index = fileForm->getModel()->index(localPath);
                    if(index.isValid())
                        fileForm->getCurrentItemView()->edit(index);
                }
            }
        });
        menu.addAction(QIcon(":/img/ic_delete_forever"),tr("delete"),[this,localPath]() {
            QFile::remove(localPath);
        });
        menu.addAction(QIcon(":/img/ic_open_folder"),tr("open in folder"),[this,localPath]() {
            QString fileDir = QFileInfo(localPath).absoluteDir().absolutePath();
            QString urlStr = "file:";
            QDesktopServices::openUrl(QUrl(urlStr.append(fileDir), QUrl::TolerantMode));
        });
        menu.addAction(tr("open default"),[this,localPath]() {
            QString urlStr = "file:";
            QDesktopServices::openUrl(QUrl(urlStr.append(localPath), QUrl::TolerantMode));
        });
        if(!deviceList.isEmpty()) {
            if(localPath.endsWith(".apk")) {
                QMenu *installApkMenu = menu.addMenu(tr("install apk to..."));
                for(int i = 0; i < deviceList.size(); i++) {
                    CAndroidDevice * device = deviceList.at(i);
                    installApkMenu->addAction(QString("%1 [%2]").arg(device->getModel()).arg(device->serialNumber),[device,localPath,this]() {
                        installApk(device->serialNumber,localPath);
                    });
                }
            }
            QMenu *pushFileMenu = menu.addMenu(tr("push file to..."));
            for(int i = 0; i < deviceList.size(); i++) {
                CAndroidDevice * device = deviceList.at(i);
                pushFileMenu->addAction(QString("%1 [%2]").arg(device->getModel()).arg(device->serialNumber),[device,localPath,this]() {
                    CDeviceFileDialog dialog(device,QFileInfo(localPath).fileName());
                    if(dialog.exec() == QDialog::Accepted) {
                        QString dPath = dialog.getChoosePath();
                        pushFile(device->serialNumber,localPath,dPath);
                    }
                });
            }
        }
    }

    if(!serialNumber.isEmpty()) {
        if(localPath.isEmpty() && devicePath.isEmpty()) {
            menu.addAction(QIcon(":/img/camera_roll"),tr("screen record"),[this,serialNumber]() {
                screenRecord(serialNumber);
            });
            menu.addAction(QIcon(":/img/ic_shot"),tr("screen shot"),[this,serialNumber]() {
                screenShot(serialNumber);
            });
            menu.addAction(QIcon(":/img/ic_reboot"),tr("reboot"),[this,serialNumber]() {
                CAndroidDevice * device = CAndroidContext::getDevice(serialNumber);
                if(device != nullptr)
                    device->reboot();
            });
            menu.addAction(QIcon(":/img/pull"),tr("pull file"),this,[this,serialNumber]() {
                CAndroidDevice * device = CAndroidContext::getDevice(serialNumber);
                CDeviceFileDialog dialog(device);
                if(dialog.exec() == QDialog::Accepted) {
                    QString dPath = dialog.getChoosePath();
                    QString lPath = QFileDialog::getSaveFileName(this, tr("Choose Local File"),
                                    QDir::homePath() + QDir::separator() + dPath.right(dPath.size() - dPath.lastIndexOf('/')),
                                    tr("All File (*.*)"));
                    pullFile(serialNumber,lPath,dPath);
                }
            });
            menu.addAction(QIcon(":/img/push"),tr("push file"),this,[this,serialNumber]() {
                QString lPath = QFileDialog::getOpenFileName(this, tr("Choose Local File"),
                                QDir::rootPath(),
                                tr("All File (*.*)"));
                if(!lPath.isEmpty()) {
                    CAndroidDevice * device = CAndroidContext::getDevice(serialNumber);
                    CDeviceFileDialog dialog(device,QFileInfo(lPath).fileName());
                    if(dialog.exec() == QDialog::Accepted) {
                        QString dPath = dialog.getChoosePath();
                        pushFile(serialNumber,lPath,dPath);
                    }
                }
            });
            menu.addAction(tr("install"),this,[this,serialNumber]() {
                QString apkFilePath = QFileDialog::getOpenFileName(this, tr("Choose Apk File"),
                                      QDir::rootPath(),
                                      tr("Android Application File (*.apk)"));
                if(!apkFilePath.isEmpty()) {
                    installApk(serialNumber,apkFilePath);
                }
            });
        } else if(!localPath.isEmpty() && devicePath.isEmpty()) {
            menu.addAction(QIcon(":/img/push"),tr("push file"),this,[this,localPath,serialNumber]() {
                CAndroidDevice * device = CAndroidContext::getDevice(serialNumber);
                CDeviceFileDialog dialog(device,QFileInfo(localPath).fileName());
                if(dialog.exec() == QDialog::Accepted) {
                    QString dPath = dialog.getChoosePath();
                    pushFile(serialNumber,localPath,dPath);
                }
            });
            if(localPath.endsWith(".apk")) {
                menu.addAction(tr("install"),this,[this,serialNumber]() {
                    QString apkFilePath = QFileDialog::getOpenFileName(this, tr("Choose Apk File"),
                                          QDir::rootPath(),
                                          tr("Android Application File (*.apk)"));
                    if(!apkFilePath.isEmpty()) {
                        installApk(serialNumber,apkFilePath);
                    }
                });
            }
        } else if(localPath.isEmpty() && !devicePath.isEmpty()) {
            menu.addAction(QIcon(":/img/pull"),tr("pull file"),[this,serialNumber,devicePath]() {
                QString lPath = QFileDialog::getSaveFileName(this, tr("Choose Local File"),
                                QDir::homePath() + QDir::separator() + devicePath.right(devicePath.size() - devicePath.lastIndexOf('/')),
                                tr("All File (*.*)"));
                pullFile(serialNumber,lPath,devicePath);
            });
        } else {
            if(this->ui->rightDockWidget->rect().contains(this->ui->rightDockWidget->mapFromGlobal(QCursor::pos()))) {
                menu.addAction(QIcon(":/img/push"),tr("push file"),this,[this,serialNumber,localPath,devicePath]() {
                    pushFile(serialNumber,localPath,devicePath);
                });
                if(localPath.endsWith(".apk")) {
                    menu.addAction(tr("install"),this,[this,serialNumber,localPath]() {
                        installApk(serialNumber,localPath);
                    });
                }
            } else {
                menu.addAction(QIcon(":/img/pull"),tr("pull file"),[this,serialNumber,localPath,devicePath]() {
                    pullFile(serialNumber,localPath,devicePath);
                });
            }
        }
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
            ProcessResult result = device->install(path);
            emit processEnd(result.exitCode,result.resultStr);
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
        emit processStart(tr("screen shotting..."),tr("screen shot %1").arg(device->getModel()));

        QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
        connect(watcher,&QFutureWatcher<void>::finished,[this,tmpPhonePath,watcher,device]() {
            QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                               QDir(QDir::homePath()).filePath("screen-shot-untitled.png"),
                               tr("Images (*.png)"));
            if(!savePath.isEmpty()) {
                emit processStart(tr("saving..."),tr("save image to %1").arg(savePath));
                QtConcurrent::run([=]() {
                    if(!device.isNull()) {
                        ProcessResult result = device->pull(tmpPhonePath,savePath);
                        emit processEnd(result.exitCode,result.resultStr);
                    } else {
                        emit processEnd(1,tr("device disconnect"));
                    }
                });
            }
            watcher->deleteLater();
        });
        watcher->setFuture(QtConcurrent::run([this,tmpPhonePath,device]() {
            QString tmpPhonePath = "/sdcard/canary-screen-shot.png";
            if(!device.isNull()) {
                ProcessResult result = device->screenShot(tmpPhonePath);
                emit processEnd(result.exitCode,result.resultStr);
            } else {
                emit processEnd(1,tr("device disconnect"));
            }
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
                    emit processStart(tr("saving..."),tr("save video to %1").arg(savePath));
                    QtConcurrent::run([=]() {
                        if(!device.isNull()) {
                            ProcessResult result = device->pull(tempPath,savePath);
                            emit processEnd(result.exitCode,result.resultStr);
                        } else {
                            emit processEnd(1,tr("device disconnect"));
                        }
                    });
                }
            });
            dialog->show();
        }
    }
}

void MainWindow::pushFile(const QString &serialNumber, const QString &localPath, const QString &devicePath)
{
    CAndroidDevice *device = CAndroidContext::getDevice(serialNumber);
    if(device != nullptr) {
        emit processStart(tr("pushing..."),tr("push %1 to %2 %3").arg(localPath).arg(device->getModel()).arg(devicePath));
        QtConcurrent::run([device,localPath,devicePath,this]() {
            ProcessResult result = device->push(localPath,devicePath);
            emit processEnd(result.exitCode,result.resultStr);
        });
    } else {
        //TODO
    }
}

void MainWindow::pullFile(const QString &serialNumber, const QString &localPath, const QString &devicePath)
{
    CAndroidDevice *device = CAndroidContext::getDevice(serialNumber);
    if(device != nullptr) {
        emit processStart(tr("pulling..."),tr("pull %1 from %2 %3").arg(localPath).arg(device->getModel()).arg(devicePath));
        QtConcurrent::run([device,localPath,devicePath,this]() {
            ProcessResult result = device->pull(devicePath,localPath);
            emit processEnd(result.exitCode,result.resultStr);
        });
    } else {
        //TODO
    }
}
