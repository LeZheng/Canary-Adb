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

    connect(ui->deviceIconLabel,&QLabel::customContextMenuRequested,[this]() {
        if(!devicePointer.isNull()) {
            emit itemMenuRequested(devicePointer);
        }
    });
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
        ui->wmSizeLineEdit->setText(device->getWmSize());
        ui->deviceIconLabel->setPixmap(QPixmap(":/img/phone"));
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
    if(event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if(urls.at(0).isLocalFile()) {
            QString filePath = urls.at(0).toLocalFile();
            QMenu menu;
            menu.addAction(tr("push"),[this]() {
                //TODO push
            });
            if(filePath.endsWith(".apk")) {
                menu.addAction(tr("install"),[this,filePath]() {
                    if(!this->devicePointer.isNull()){
                        //TODO worker thread
                        devicePointer->install(filePath);
                    }
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
            && !devicePointer.isNull())//判断是否执行拖动
    {
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
