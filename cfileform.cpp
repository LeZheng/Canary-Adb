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
    connect(ui->fileTreeView,&QTreeView::customContextMenuRequested,this,[this](const QPoint &pos){
        QModelIndex index = this->ui->fileTreeView->indexAt(pos);
        if(!index.isValid()) {
            return;
        }
        QString path = this->model->filePath(index);
        emit menuRequested(path);
    });
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
    connect(ui->browserBtn,&QPushButton::clicked,[this](){
        QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                         QDir::rootPath(),
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
        if(!path.isEmpty()){
            this->ui->pathLineEdit->setText(path);
            this->fileTreeView->setRootIndex(this->model->index(path));
            this->historyPathStack.clear();
            this->ui->nextBtn->setEnabled(false);
        }
    });
}

CFileForm::~CFileForm()
{
    delete ui;
}
