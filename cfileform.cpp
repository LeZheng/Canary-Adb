#include "cfileform.h"
#include "ui_cfileform.h"

#include <QDebug>
#include <QDrag>
#include <QMimeData>

CFileForm::CFileForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFileForm)
{
    ui->setupUi(this);

    fileTreeView = ui->fileTreeView;
    workPath = QDir::rootPath();
    model = new QFileSystemModel(this);
    model->setRootPath(workPath);
    model->setReadOnly(false);
    ui->fileTreeView->setModel(model);
    QStyle * style = QApplication::style();
    ui->prevBtn->setIcon(style->standardIcon(QStyle::SP_ArrowLeft));
    ui->nextBtn->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
    ui->browserBtn->setIcon(style->standardIcon(QStyle::SP_DirOpenIcon));
    ui->fileTreeView->header()->setVisible(false);
    ui->pathLineEdit->setReadOnly(true);
    for(int i = 1; i < model->columnCount(); i++) {
        ui->fileTreeView->hideColumn(i);
    }
    connect(ui->fileTreeView,&QTreeView::doubleClicked,[this](const QModelIndex &index) {
        QString clickPath = this->model->filePath(index);
        if(this->model->isDir(index)) {
            this->historyPathStack.clear();
            this->ui->nextBtn->setEnabled(false);
            this->ui->fileTreeView->setRootIndex(index);
            this->ui->pathLineEdit->setText(clickPath);
        } else {
            QString urlStr = "file:";
            QDesktopServices::openUrl(QUrl(urlStr.append(clickPath), QUrl::TolerantMode));
        }
    });
    connect(ui->fileTreeView,&QTreeView::customContextMenuRequested,this,&CFileForm::showFileRequestMenu);
    connect(this->ui->prevBtn,&QPushButton::clicked,[this]() {
        QModelIndex pIndex = this->ui->fileTreeView->rootIndex();
        QModelIndex index = pIndex.parent();
        if(index.isValid()) {
            QString path = this->model->filePath(index);
            this->ui->pathLineEdit->setText(path);
            this->ui->fileTreeView->setRootIndex(index);
            this->historyPathStack.push(pIndex);
            this->ui->nextBtn->setEnabled(true);
        }
    });
    connect(ui->nextBtn,&QPushButton::clicked,[this]() {
        if(!this->historyPathStack.isEmpty()) {
            QModelIndex index = this->historyPathStack.pop();
            this->ui->fileTreeView->setRootIndex(index);
            this->ui->pathLineEdit->setText(this->model->filePath(index));
            if(this->historyPathStack.isEmpty())
                this->ui->nextBtn->setEnabled(false);
        }
    });
}

CFileForm::~CFileForm()
{
    delete ui;
}

void CFileForm::showFileRequestMenu(const QPoint &pos)
{
    QModelIndex index = this->ui->fileTreeView->indexAt(pos);
    if(!index.isValid()) {
        return;
    }
    QString path = this->model->filePath(index);

    QMenu menu;
    menu.addAction(tr("rename"),[this,index]() {
        this->getTreeView()->edit(index);
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
        QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
        if(!deviceList.isEmpty()) {
            QMenu *installApkMenu = menu.addMenu(path.endsWith(".apk") ? tr("install apk to...") : tr("push file to..."));
            for(int i = 0; i < deviceList.size(); i++) {
                CAndroidDevice * device = deviceList.at(i);
                if(path.endsWith(".apk")) {
                    installApkMenu->addAction(tr("%1 [%2]").arg(device->getModel()).arg(device->serialNumber),[device,path,this]() {
                        emit processStart(tr("installing..."),tr("install %1 to %2").arg(path).arg(device->getModel()));
                        QtConcurrent::run([device,path,this]() {
                            device->install(path);
                            emit processEnd(0,"");
                        });
                    });
                } else {
                    installApkMenu->addAction(tr("%1 [%2]").arg(device->getModel()).arg(device->serialNumber),[device,path,this]() {
                        //TODO choose push path
                        QString desPath;
                        emit processStart(tr("pushing..."),tr("push %1 to %2").arg(path).arg(device->getModel()));
                        QtConcurrent::run([device,path,desPath,this]() {
                            device->push(path,desPath);
                            emit processEnd(0,"");
                        });
                    });
                }
            }
        }
    }
    menu.exec(QCursor::pos());
}
