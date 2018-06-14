#include "cdevicefileform.h"
#include "ui_cdevicefileform.h"

CDeviceFileForm::CDeviceFileForm(CAndroidDevice * device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceFileForm)
{
    ui->setupUi(this);
    devicePointer = device;
    this->deviceSerialNumber = device->serialNumber;

    this->ui->fileTableWidget->setColumnCount(5);
    QStringList headers;
    headers << tr("file name") << tr("group") << tr("owner") << tr("size") << tr("time");
    this->ui->fileTableWidget->setHorizontalHeaderLabels(headers);

    if(!this->devicePointer.isNull()){
        openDir(currentDir);
    }

    connect(ui->fileTableWidget,&QTableWidget::cellDoubleClicked,this,[this](int row, int column){
        if(row < this->currentFiles.size()){
            CAndroidFile file = this->currentFiles.at(row);
            if(file.privilege.startsWith('d')){
                QString nextPath = currentDir + "/" + file.fileName;
                prevPathStack.push(currentDir);
                nextPathStack.clear();
                openDir(nextPath);
            }
        }
    });

    ui->nextToolButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    ui->nextToolButton->setFixedSize(QSize(24,24));
    connect(ui->nextToolButton,&QToolButton::clicked,this,[this](){
        if(!nextPathStack.isEmpty()){
            QString path = nextPathStack.pop();
            prevPathStack.push(this->currentDir);
            openDir(path);
        }
    });

    ui->prevToolButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->prevToolButton->setFixedSize(QSize(24,24));
    connect(ui->prevToolButton,&QToolButton::clicked,this,[this](){
        if(!prevPathStack.isEmpty()){
            QString path = prevPathStack.pop();
            nextPathStack.push(this->currentDir);
            openDir(path);
        }
    });

    connect(ui->pathLineEdit,&QLineEdit::textChanged,this,[this](){
        this->ui->prevToolButton->setDisabled(prevPathStack.isEmpty());
        this->ui->nextToolButton->setDisabled(nextPathStack.isEmpty());
    });
}

CDeviceFileForm::~CDeviceFileForm()
{
    delete ui;
}

void CDeviceFileForm::openDir(const QString &path)
{
    currentFiles = this->devicePointer->listDir(path);

    this->ui->fileTableWidget->clearContents();
    this->ui->fileTableWidget->setRowCount(currentFiles.length());
    for(int i = 0;i < currentFiles.length();i++){
        CAndroidFile file = currentFiles.at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(QIcon(),file.fileName);
        if(file.privilege.startsWith('d')){
            nameItem->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon)));
        }else{
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
        ui->pathLineEdit->setText(currentDir.isEmpty() ? "/" : currentDir);
    }
}
