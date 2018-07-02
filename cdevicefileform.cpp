#include "cdevicefileform.h"
#include "ui_cdevicefileform.h"

CDeviceFileForm::CDeviceFileForm(CAndroidDevice * device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceFileForm)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    devicePointer = device;
    this->deviceSerialNumber = device->serialNumber;
    this->ui->fileTableWidget->setSerialNumber(device->serialNumber);

    this->ui->fileTableWidget->setColumnCount(5);
    QStringList headers;
    headers << tr("file name") << tr("group") << tr("owner") << tr("size") << tr("time");
    this->ui->fileTableWidget->setHorizontalHeaderLabels(headers);

    if(!this->devicePointer.isNull()) {
        openDir(currentDir);
    }

    connect(ui->fileTableWidget,&QTableWidget::cellDoubleClicked,this,[this](int row, int column) {
        if(row < this->currentFiles.size()) {
            CAndroidFile file = this->currentFiles.at(row);
            if(file.privilege.startsWith('d')) {
                QString nextPath = currentDir + "/" + file.fileName;
                prevPathStack.push(currentDir);
                nextPathStack.clear();
                openDir(nextPath);
            }
        }
    });

    connect(ui->fileTableWidget,&QTableWidget::customContextMenuRequested,this,[this](const QPoint &pos){
        int row = ui->fileTableWidget->rowAt(pos.y());
        if(row < currentFiles.size() && row >= 0){
            emit menuRequested(this->deviceSerialNumber,"",currentFiles.at(row).path);
        }
    });

    ui->nextToolButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    ui->nextToolButton->setFixedSize(QSize(24,24));
    connect(ui->nextToolButton,&QToolButton::clicked,this,[this]() {
        if(!nextPathStack.isEmpty()) {
            QString path = nextPathStack.pop();
            prevPathStack.push(this->currentDir);
            openDir(path);
        }
    });

    ui->prevToolButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->prevToolButton->setFixedSize(QSize(24,24));
    connect(ui->prevToolButton,&QToolButton::clicked,this,[this]() {
        if(!prevPathStack.isEmpty()) {
            QString path = prevPathStack.pop();
            nextPathStack.push(this->currentDir);
            openDir(path);
        }
    });

    connect(ui->pathLineEdit,&QLineEdit::textChanged,this,[this]() {
        this->ui->prevToolButton->setDisabled(prevPathStack.isEmpty());
        this->ui->nextToolButton->setDisabled(nextPathStack.isEmpty());
    });

    ui->refreshToolButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    connect(ui->refreshToolButton,&QToolButton::clicked,this,[this](){
        openDir(currentDir);
    });
}

CDeviceFileForm::~CDeviceFileForm()
{
    delete ui;
}

void CDeviceFileForm::dragEnterEvent(QDragEnterEvent *event)
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

void CDeviceFileForm::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls() && !this->devicePointer.isNull()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if(urls.at(0).isLocalFile()) {
            QString localPath = urls.at(0).toLocalFile();
            int dy = event->pos().y() - (ui->fileTableWidget->y() - this->y() + ui->fileTableWidget->horizontalHeader()->height());
            int row = ui->fileTableWidget->rowAt(dy);
            if(row >= 0) {
                CAndroidFile deviceFile = currentFiles.at(row);
                emit menuRequested(this->devicePointer->serialNumber,localPath,deviceFile.path);
            }else{
                emit menuRequested(this->devicePointer->serialNumber,localPath,currentDir);
            }
        }
    }
}

void CDeviceFileForm::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    mStartPoint = event->pos();
}

void CDeviceFileForm::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    if (ui->fileTableWidget->rect().contains(mStartPoint) &&
        (event->pos() - mStartPoint).manhattanLength() > QApplication::startDragDistance()
        && !devicePointer.isNull()) { //判断是否执行拖动
        //TODO drag out
    }
}

void CDeviceFileForm::openDir(const QString &path)
{
    currentFiles = this->devicePointer->listDir(path);

    this->ui->fileTableWidget->clearContents();
    this->ui->fileTableWidget->setRowCount(currentFiles.length());
    for(int i = 0; i < currentFiles.length(); i++) {
        CAndroidFile file = currentFiles.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(QIcon(),file.fileName);
        if(file.privilege.startsWith('d')) {
            nameItem->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon)));
        } else {
            nameItem->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon)));
        }
        QTableWidgetItem *groupItem = new QTableWidgetItem(file.group);
        QTableWidgetItem *ownerItem = new QTableWidgetItem(file.owner);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(file.size);
        QTableWidgetItem *timeItem = new QTableWidgetItem(file.time);
        this->ui->fileTableWidget->setItem(i,0,nameItem);
        this->ui->fileTableWidget->setItem(i,1,groupItem);
        this->ui->fileTableWidget->setItem(i,2,ownerItem);
        this->ui->fileTableWidget->setItem(i,3,sizeItem);
        this->ui->fileTableWidget->setItem(i,4,timeItem);
        this->currentDir = path;
        this->ui->fileTableWidget->setBasePath(currentDir);
        ui->pathLineEdit->setText(currentDir.isEmpty() ? "/" : currentDir);
    }
}

CDeviceFileTableWidget::CDeviceFileTableWidget(QWidget *parent):QTableWidget(parent)
{

}

QMimeData *CDeviceFileTableWidget::mimeData(const QList<QTableWidgetItem *> items) const
{
    QMimeData *data = new QMimeData;
    QListIterator<QTableWidgetItem *> iter(items);
    while(iter.hasNext()){
        QTableWidgetItem *item = iter.next();
        if(item->column() == 0){
            data->setText("android:" + serialNumber);
            QList<QUrl> urls;
            urls.append(QUrl(basePath + "/" + item->text()));
            data->setUrls(urls);
            break;
        }
    }
    return data;
}
