#include "cdeviceeditform.h"
#include "ui_cdeviceeditform.h"

CDeviceEditForm::CDeviceEditForm(CAndroidDevice * device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceEditForm)
{
    ui->setupUi(this);
    devicePointer = device;
    this->deviceSerialNumber = device->serialNumber;
    screenSyncProcess = new QProcess(this);

    connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListUpdated,this,[this]() {
        QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
        QListIterator<CAndroidDevice *> deviceIter(deviceList);
        while(deviceIter.hasNext()) {
            CAndroidDevice * device = deviceIter.next();
            if(device->serialNumber == this->deviceSerialNumber) {
                this->setEnabled(true);
                this->devicePointer = device;
                return;
            }
        }
        this->setDisabled(true);
        this->ui->syncScreenCheckBox->setChecked(false);
        this->ui->syncTouchCheckBox->setChecked(false);
    });

    connect(ui->powerButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(26);
    });
    connect(ui->menuButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(82);
    });
    connect(ui->homeButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(3);
    });
    connect(ui->goBackButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(4);
    });
    connect(ui->screenOnButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(224);
    });
    connect(ui->screenOffButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(223);
    });
    connect(ui->volumeHighButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(24);
    });
    connect(ui->volumeLowButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(25);
    });
    connect(ui->volumeMuteButton,&QToolButton::clicked,this,[this]() {
        this->inputKeyEvent(164);
    });
    connect(ui->screenShotButton,&QToolButton::clicked,this,[this]() {
        QString tmpPhonePath = "/sdcard/canary-screen-shot.png";
        emit processStart(tr("screen shot..."),tr("screen shot %1").arg(this->devicePointer->getModel()));

        QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
        connect(watcher,&QFutureWatcher<void>::finished,[this,tmpPhonePath,watcher]() {
            QString savePath = QFileDialog::getSaveFileName(this, tr("Save File"),
                               QDir(QDir::homePath()).filePath("screen-shot-untitled.png"),
                               tr("Images (*.png)"));
            if(!savePath.isEmpty()) {
                emit processStart(tr("save..."),tr("save image to %1").arg(savePath));
                QtConcurrent::run([=]() {
                    this->devicePointer->pull(tmpPhonePath,savePath);
                    emit processEnd(0,"");
                });
            }
            watcher->deleteLater();
        });
        watcher->setFuture(QtConcurrent::run([this,tmpPhonePath]() {
            QString tmpPhonePath = "/sdcard/canary-screen-shot.png";
            this->devicePointer->screenShot(tmpPhonePath);
            emit processEnd(0,"");
        }));
    });
    connect(ui->screenRecordButton,&QToolButton::clicked,this,[this]() {
        QString tempPath = "/sdcard/canary-screen-record.mp4";
        ScreenRecordOptionDialog dialog(this->devicePointer,this);
        int resultCode = dialog.exec();
        if(resultCode == QDialog::Accepted) {
            int bitRate = dialog.getBitRate() * 1000000;
            QString size = dialog.getSize();
            QProcess * process = this->devicePointer->screenRecord(tempPath,size,bitRate);
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
                        this->devicePointer->pull(tempPath,savePath);
                        emit processEnd(0,"");
                    });
                }
            });
            dialog->show();
        }
    });
    scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);
    this->scene->setSceneRect(0,0,360,640);
    screenItem = new QGraphicsPixmapItem();
    this->scene->addItem(screenItem);

    connect(ui->syncScreenCheckBox,&QCheckBox::stateChanged,this,[this](int state){
        if(state == Qt::Checked){
            QtConcurrent::run([this]() {
                while(this->ui->syncScreenCheckBox->checkState() == Qt::Checked && !this->devicePointer.isNull()) {
                    QByteArray imgBuf = this->devicePointer->screenShot();
                    bool loadOk = this->screenPixmap.loadFromData(imgBuf,"PNG");
                    if(loadOk) {
                        emit screenUpdated();
                    } else {
                        qDebug() << "load failed";
                    }
                }
            });
        }
    });

    connect(this,&CDeviceEditForm::screenUpdated,this,[this]() {
        this->screenItem->setPixmap(this->screenPixmap);
    });

    connect(ui->zoomInButton,&QToolButton::clicked,this,[this]() {
        this->scenePercent = this->scenePercent + 25;
        this->ui->zoomLabel->setText(tr("%1%").arg(this->scenePercent));
        QSize wmSize = this->devicePointer->getWmSize();
        this->scene->setSceneRect(0,0,wmSize.width() * this->scenePercent / 100,wmSize.height() * this->scenePercent / 100);
    });
    connect(ui->zoomOutButton,&QToolButton::clicked,this,[this]() {
        this->scenePercent = this->scenePercent - 25;
        this->ui->zoomLabel->setText(tr("%1%").arg(this->scenePercent));
        QSize wmSize = this->devicePointer->getWmSize();
        this->scene->setSceneRect(0,0,wmSize.width() * this->scenePercent / 100,wmSize.height() * this->scenePercent / 100);
    });
    connect(this->scene,&QGraphicsScene::sceneRectChanged,this,[this](const QRectF &rect) {
        if(this->scenePercent <= 25) {
            this->ui->zoomOutButton->setDisabled(true);
        } else {
            this->ui->zoomOutButton->setEnabled(true);
        }
        this->screenItem->setScale(this->scenePercent / 100.0f);
    });
    this->ui->zoomOutButton->setDisabled(true);
    this->screenItem->setScale(this->scenePercent / 100.0f);
}

CDeviceEditForm::~CDeviceEditForm()
{
    this->disconnect();
    delete ui;
}

void CDeviceEditForm::inputKeyEvent(int keyCode)
{
    emit processStart(tr("input..."),tr("input key event %1 to %2").arg(keyCode).arg(this->devicePointer->getModel()));
    QtConcurrent::run([keyCode,this]() {
        this->devicePointer->inputKeyEvent(keyCode);
        emit processEnd(0,"");
    });
}
