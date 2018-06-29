#include "cdevicefiledialog.h"
#include "ui_cdevicefiledialog.h"

CDeviceFileDialog::CDeviceFileDialog(CAndroidDevice * device,const QString &defaultName,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDeviceFileDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("choose path"));
    devicePointer = device;
    this->deviceSerialNumber = device->serialNumber;

    this->ui->nameLineEdit->setText(defaultName);
    this->ui->fileTableWidget->setColumnCount(5);
    QStringList headers;
    headers << tr("file name") << tr("group") << tr("owner") << tr("size") << tr("time");
    this->ui->fileTableWidget->setHorizontalHeaderLabels(headers);

    if(!this->devicePointer.isNull()) {
        openDir(currentDir);
    }

    connect(ui->fileTableWidget,&QTableWidget::cellDoubleClicked,this,[this,defaultName](int row, int column) {
        if(row < this->currentFiles.size()) {
            CAndroidFile file = this->currentFiles.at(row);
            if(file.privilege.startsWith('d')) {
                QString nextPath = currentDir + "/" + file.fileName;
                prevPathStack.push(currentDir);
                nextPathStack.clear();
                openDir(nextPath);
                currentFileName = "";
                this->ui->nameLineEdit->setText(defaultName);
            } else {
                this->currentFileName = file.fileName;
                this->ui->nameLineEdit->setText(file.fileName);
                accept();
            }
        }
    });

    connect(ui->fileTableWidget,&QTableWidget::cellClicked,this,[this](int row, int column) {
        if(row < this->currentFiles.size()) {
            CAndroidFile file = this->currentFiles.at(row);
            this->currentFileName = file.fileName;
            if(!file.privilege.startsWith("d")){
                this->ui->nameLineEdit->setText(file.fileName);
            }
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

    connect(ui->nameLineEdit,&QLineEdit::textChanged,this,[this](){
        if(this->ui->nameLineEdit->text().isEmpty()){
            this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            this->ui->nameLineEdit->setClearButtonEnabled(false);
        }else{
            this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            this->ui->nameLineEdit->setClearButtonEnabled(true);
        }
    });
}

CDeviceFileDialog::~CDeviceFileDialog()
{
    delete ui;
}

QString CDeviceFileDialog::getChoosePath()
{
    if(currentFileName != ui->nameLineEdit->text()){
        return currentDir + "/" + currentFileName + "/" + ui->nameLineEdit->text();
    }else{
        return currentDir + "/" + currentFileName;
    }
}

void CDeviceFileDialog::openDir(const QString &path)
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
        ui->pathLineEdit->setText(currentDir.isEmpty() ? "/" : currentDir);
    }
}
