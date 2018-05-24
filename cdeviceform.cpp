#include "cdeviceform.h"
#include "ui_cdeviceform.h"

CDeviceForm::CDeviceForm(CAndroidDevice * device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceForm)
{
    ui->setupUi(this);
    devicePointer = device;
    updateDevices();

    setAcceptDrops(true);

    connect(ui->deviceIconLabel,&QLabel::customContextMenuRequested,this,&CDeviceForm::showDeviceRequestMenu);
}

CDeviceForm::~CDeviceForm()
{
    delete ui;
}

void CDeviceForm::updateDevices()
{
    if(!devicePointer.isNull()) {
        CAndroidDevice * device = devicePointer;
        ui->androidIdLineEdit->setText(device->getAndroidId());
        ui->androidVersionLineEdit->setText(device->getAndroidVersion());
        ui->deviceNameLineEdit->setText(device->getModel());
        ui->wmDensityLineEdit->setText(device->getWmDensity());
        QSize wmSize = device->getWmSize();
        ui->wmSizeLineEdit->setText(tr("%1 x %2").arg(wmSize.width()).arg(wmSize.height()));
        ui->deviceIconLabel->setPixmap(QPixmap(":/img/phone"));
    }
}

void CDeviceForm::showDeviceRequestMenu()
{
    if(!this->devicePointer.isNull()) {
        QMenu menu;
        menu.addAction(tr("open detail"),[this]() {
            emit requestOpenDetail(this->devicePointer);
        });
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
}

void CDeviceForm::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if(urls.size() == 1) { //TODO larger than 1 ??
            QListIterator<QUrl> iter(urls);
            while(iter.hasNext()) {
                if(iter.next().isLocalFile()) {
                    event->accept();
                    return;
                }
            }
        }
    }
}

void CDeviceForm::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls() && !this->devicePointer.isNull()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if(urls.at(0).isLocalFile()) {
            QString filePath = urls.at(0).toLocalFile();
            QMenu menu;
            if(filePath.endsWith(".apk")) {
                menu.addAction(tr("install"),[this,filePath]() {
                    emit processStart(tr("installing..."),tr("install %1 to %2").arg(filePath).arg(this->devicePointer->getModel()));
                    QtConcurrent::run([filePath,this]() {
                        this->devicePointer->install(filePath);
                        emit processEnd(0,"");
                    });
                });
            } else {
                menu.addAction(tr("push"),[this]() {
                    //TODO push
                });
            }
            menu.exec(QCursor::pos());
        }
    }
}

void CDeviceForm::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    mStartPoint = event->pos();
}

void CDeviceForm::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if ((event->pos() - mStartPoint).manhattanLength() > QApplication::startDragDistance()
        && !devicePointer.isNull()) { //判断是否执行拖动
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(devicePointer->serialNumber);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(":/img/phone_small"));
        drag->setHotSpot(QPoint(16,16));
        drag->exec(Qt::MoveAction);
        delete drag;
    }
}
