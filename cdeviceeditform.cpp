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

    connect(ui->menuButton,&QToolButton::clicked,ui->actionmenu,&QAction::trigger);
    connect(ui->actionmenu,&QAction::triggered,this,[this]() {
        this->inputKeyEvent(187);
    });
    connect(ui->homeButton,&QToolButton::clicked,ui->actionhome,&QAction::trigger);
    connect(ui->actionhome,&QAction::triggered,this,[this]() {
        this->inputKeyEvent(3);
    });
    connect(ui->goBackButton,&QToolButton::clicked,ui->actionback,&QAction::trigger);
    connect(ui->actionback,&QAction::triggered,this,[this]() {
        this->inputKeyEvent(4);
    });

    connect(ui->actionscreen_on,&QAction::triggered,this,[this]() {
        this->inputKeyEvent(224);
    });
    connect(ui->actionscreen_off,&QAction::triggered,this,[this]() {
        this->inputKeyEvent(223);
    });
    connect(ui->actionvolume_up,&QAction::triggered,this,[this]() {
        this->inputKeyEvent(24);
    });
    connect(ui->actionvolume_down,&QAction::triggered,this,[this]() {
        this->inputKeyEvent(25);
    });
    connect(ui->actionvolume_mute,&QAction::triggered,this,[this]() {
        this->inputKeyEvent(164);
    });

    for(int i = 0; i < 10; i++) {
        int number = i;
        QAction *actNumber = new QAction(tr("input number %1").arg(number),this);
        connect(actNumber,&QAction::triggered,this,[this,number](){
            this->inputKeyEvent(number + 7);
        });
        inputNumberActions.append(actNumber);
    }

    for(int i = 0; i < 26; i++) {
        char letter = i + 65;
        QAction *actLetter = new QAction(tr("input letter %1").arg(letter),this);
        connect(actLetter,&QAction::triggered,this,[this,letter](){
            this->inputKeyEvent(letter - 36);
        });
        inputLetterActions.append(actLetter);
    }


    scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);
    QSize wmSize = this->devicePointer->getWmSize();
    this->scene->setSceneRect(0,0,wmSize.width() * this->scenePercent / 100,wmSize.height() * this->scenePercent / 100);
    screenItem = new CScreenPixmapItem();
    this->scene->addItem(screenItem);

    connect(screenItem,&CScreenPixmapItem::itemClick,this,[this](const QPoint &pos) {
        if(this->ui->syncTouchCheckBox->checkState() == Qt::Checked) {
            emit processStart(tr("input..."),tr("input click event to %1").arg(this->devicePointer->serialNumber));
            QtConcurrent::run([=]() {
                ProcessResult result = this->devicePointer->inputClick(pos);
                emit processEnd(result.exitCode,result.resultStr);
            });
        }
    });

    connect(screenItem,&CScreenPixmapItem::itemLongClick,this,[this](const QPoint &pos,int duration) {
        if(this->ui->syncTouchCheckBox->checkState() == Qt::Checked) {
            emit processStart(tr("input..."),tr("input long click event to %1").arg(this->devicePointer->serialNumber));
            QtConcurrent::run([=]() {
                ProcessResult result = this->devicePointer->inputSwipe(pos,pos,duration > 0 ? duration * 1000 : 1000);
                emit processEnd(result.exitCode,result.resultStr);
            });
        }
    });

    connect(screenItem,&CScreenPixmapItem::itemSwipe,this,[this](const QPoint &startPos,const QPoint &endPos,int duration) {
        if(this->ui->syncTouchCheckBox->checkState() == Qt::Checked) {
            emit processStart(tr("input..."),tr("input swipe event to %1").arg(this->devicePointer->serialNumber));
            QtConcurrent::run([=]() {
                ProcessResult result = this->devicePointer->inputSwipe(startPos,endPos,duration > 0 ? duration * 1000 : 1000);
                emit processEnd(result.exitCode,result.resultStr);
            });
        }
    });

    connect(ui->syncScreenCheckBox,&QCheckBox::stateChanged,this,[this](int state) {
        if(state == Qt::Checked) {
            QtConcurrent::run([this]() {
                bool canLoadFromOutput = true;
                bool needHandleNewLine = false;
                while(this->ui->syncScreenCheckBox->checkState() == Qt::Checked && !this->devicePointer.isNull() && this->isVisible()) {
                    if(canLoadFromOutput) {
                        QByteArray imgBuf = this->devicePointer->screenShot();
                        if(this->isVisible()) {
                            if(!needHandleNewLine) {
                                canLoadFromOutput = this->screenPixmap.loadFromData(imgBuf,"PNG");
                                needHandleNewLine = !canLoadFromOutput;
                            }
                            if(needHandleNewLine) {
                                imgBuf.replace("\r\n","\n");
                                canLoadFromOutput = this->screenPixmap.loadFromData(imgBuf,"PNG");
                            }
                        } else {
                            return;
                        }
                    }
                    if(canLoadFromOutput) {
                        emit screenUpdated();
                    } else {
                        QString tmpPhonePath = "/sdcard/canary-screen-tmp.png";
                        QString destPath = QDir::temp().absoluteFilePath("canary-screen-tmp.png");
                        if(!this->devicePointer.isNull())
                            this->devicePointer->screenShot(tmpPhonePath);
                        if(!this->devicePointer.isNull())
                            this->devicePointer->pull(tmpPhonePath,destPath);
                        if(this->isVisible()) {
                            bool loadOk = this->screenPixmap.load(destPath,"PNG");
                            if(loadOk) {
                                emit screenUpdated();
                            } else {
                                qDebug() << "load failed again!";
                            }
                        } else {
                            return;
                        }
                    }
                }
            });
        }
    });

    connect(this,&CDeviceEditForm::screenUpdated,this,[this]() {
        this->screenItem->setPixmap(this->screenPixmap);
    });

    connect(ui->zoomSlider,&QSlider::valueChanged,this,[this](int value) {
        this->scenePercent = value;
        this->ui->zoomLabel->setText(QString("%1%").arg(this->scenePercent));
        QSize wmSize = this->devicePointer->getWmSize();
        this->scene->setSceneRect(0,0,wmSize.width() * this->scenePercent / 100,wmSize.height() * this->scenePercent / 100);
    });

    connect(this->scene,&QGraphicsScene::sceneRectChanged,this,[this](const QRectF &rect) {
        this->screenItem->setScale(this->scenePercent / 100.0f);
    });

    this->screenItem->setScale(this->scenePercent / 100.0f);
    this->ui->zoomSlider->setValue(this->scenePercent);
}

CDeviceEditForm::~CDeviceEditForm()
{
    this->ui->syncScreenCheckBox->setChecked(false);
    this->ui->syncTouchCheckBox->setChecked(false);
    this->disconnect();
    delete ui;
}

void CDeviceEditForm::closeEvent(QCloseEvent *event)
{
    this->ui->syncScreenCheckBox->setChecked(false);
    this->ui->syncTouchCheckBox->setChecked(false);
}

void CDeviceEditForm::wheelEvent(QWheelEvent *event)
{
    if((QApplication::keyboardModifiers() == Qt::ControlModifier)) {
        ui->zoomSlider->setValue(ui->zoomSlider->value() + (event->delta() > 0 ? 5 : -5));
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

void CDeviceEditForm::inputKeyEvent(int keyCode)
{
    emit processStart(tr("input..."),tr("input key event %1 to %2").arg(keyCode).arg(this->devicePointer->getModel()));
    QtConcurrent::run([keyCode,this]() {
        ProcessResult result = this->devicePointer->inputKeyEvent(keyCode);
        emit processEnd(result.exitCode,result.resultStr);
    });
}

void CScreenPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}

void CScreenPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressTime = QTime::currentTime();
    pressPoint = event->pos().toPoint();
}

void CScreenPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QTime releaseTime = QTime::currentTime();
    if((event->pos().toPoint() - this->pressPoint).manhattanLength() > 2) {
        emit itemSwipe(this->pressPoint,event->pos().toPoint(),this->pressTime.secsTo(releaseTime));
    } else {
        if(this->pressTime.secsTo(releaseTime) >= 1) {
            emit itemLongClick(event->pos().toPoint(),this->pressTime.secsTo(releaseTime));
        } else {
            emit itemClick(event->pos().toPoint());
        }
    }
}

void CDeviceEditForm::on_graphicsView_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    menu.insertAction(nullptr,ui->actionscreen_shot);
    menu.insertAction(nullptr,ui->actionscreen_record);
    menu.insertAction(nullptr,ui->actionscreen_on);
    menu.insertAction(nullptr,ui->actionscreen_off);
    menu.insertAction(nullptr,ui->actionvolume_up);
    menu.insertAction(nullptr,ui->actionvolume_down);
    menu.insertAction(nullptr,ui->actionvolume_mute);

    QMenu *inputNumberMenu = menu.addMenu(tr("input number"));
    QListIterator<QAction *> numberActionIter(inputNumberActions);
    while(numberActionIter.hasNext()) {
        inputNumberMenu->insertAction(nullptr,numberActionIter.next());
    }

    QMenu *inputLetterMenu = menu.addMenu(tr("input letter"));
    QListIterator<QAction *> letterActionIter(inputLetterActions);
    while(letterActionIter.hasNext()) {
        inputLetterMenu->insertAction(nullptr,letterActionIter.next());
    }

    menu.exec(QCursor::pos());
}

void CDeviceEditForm::on_actionscreen_record_triggered()
{
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
                    ProcessResult result = this->devicePointer->pull(tempPath,savePath);
                    emit processEnd(result.exitCode,result.resultStr);
                });
            }
        });
        dialog->show();
    }
}

void CDeviceEditForm::on_actionscreen_shot_triggered()
{
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
                ProcessResult result = this->devicePointer->pull(tmpPhonePath,savePath);
                emit processEnd(result.exitCode,result.resultStr);
            });
        }
        watcher->deleteLater();
    });
    watcher->setFuture(QtConcurrent::run([this,tmpPhonePath]() {
        QString tmpPhonePath = "/sdcard/canary-screen-shot.png";
        ProcessResult result = this->devicePointer->screenShot(tmpPhonePath);
        emit processEnd(result.exitCode,result.resultStr);
    }));
}
