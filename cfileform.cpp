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
    ui->fileListView->setModel(model);
    QStyle * style = QApplication::style();

    ui->pathLineEdit->setReadOnly(true);
    for(int i = 1; i < model->columnCount(); i++) {
        ui->fileTreeView->hideColumn(i);
    }
    connect(ui->fileListView,&QListView::doubleClicked,ui->fileTreeView,&QTreeView::doubleClicked);
    connect(ui->fileTreeView,&QTreeView::doubleClicked,[this](const QModelIndex &index) {
        QString clickPath = this->model->filePath(index);
        if(this->model->isDir(index)) {
            this->historyPathStack.clear();
            this->ui->nextBtn->setEnabled(false);
            this->ui->fileTreeView->setRootIndex(index);
            this->ui->fileListView->setRootIndex(index);
            this->ui->pathLineEdit->setText(clickPath);
            emit basePathChanged(this->model->filePath(index));
        } else {
            QString urlStr = "file:";
            QDesktopServices::openUrl(QUrl(urlStr.append(clickPath), QUrl::TolerantMode));
        }
    });
    connect(ui->fileListView,&QListView::customContextMenuRequested,ui->fileTreeView,&QTreeView::customContextMenuRequested);
    connect(ui->fileTreeView,&QTreeView::customContextMenuRequested,this,[this](const QPoint &pos) {
        QModelIndex index = this->ui->fileTreeView->indexAt(pos);
        if(!index.isValid()) {
            return;
        }
        QString path = this->model->filePath(index);
        emit menuRequested(path);
    });

    ui->prevBtn->setIcon(style->standardIcon(QStyle::SP_ArrowLeft));
    ui->prevBtn->setFixedSize(QSize(24,24));
    connect(this->ui->prevBtn,&QPushButton::clicked,[this]() {
        QModelIndex pIndex = this->ui->fileTreeView->rootIndex();
        QModelIndex index = pIndex.parent();
        if(index.isValid()) {
            QString path = this->model->filePath(index);
            this->ui->pathLineEdit->setText(path);
            this->ui->fileTreeView->setRootIndex(index);
            this->ui->fileListView->setRootIndex(index);
            this->historyPathStack.push(pIndex);
            emit basePathChanged(this->model->filePath(index));
        }
    });
    ui->nextBtn->setIcon(style->standardIcon(QStyle::SP_ArrowRight));
    ui->nextBtn->setFixedSize(QSize(24,24));
    connect(ui->nextBtn,&QPushButton::clicked,[this]() {
        if(!this->historyPathStack.isEmpty()) {
            QModelIndex index = this->historyPathStack.pop();
            this->ui->fileTreeView->setRootIndex(index);
            this->ui->fileListView->setRootIndex(index);
            this->ui->pathLineEdit->setText(this->model->filePath(index));
            emit basePathChanged(this->model->filePath(index));
        }
    });
    ui->browserBtn->setIcon(style->standardIcon(QStyle::SP_DirOpenIcon));
    ui->browserBtn->setFixedSize(QSize(24,24));
    connect(ui->browserBtn,&QPushButton::clicked,[this]() {
        QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                       QDir::rootPath(),
                       QFileDialog::ShowDirsOnly
                       | QFileDialog::DontResolveSymlinks);
        if(!path.isEmpty()) {
            QModelIndex index = this->model->index(path);
            this->ui->pathLineEdit->setText(path);
            this->fileTreeView->setRootIndex(index);
            this->ui->fileListView->setRootIndex(index);
            this->historyPathStack.clear();
            emit basePathChanged(this->model->filePath(index));
        }
    });

    connect(this,&CFileForm::basePathChanged,this,[this](){
        QModelIndex pIndex = this->ui->fileTreeView->rootIndex().parent();
        if(pIndex.isValid()) {
            this->ui->prevBtn->setEnabled(true);
        }else{
            this->ui->prevBtn->setDisabled(true);
        }
        if(this->historyPathStack.isEmpty()){
            this->ui->nextBtn->setDisabled(true);
        }else{
            this->ui->nextBtn->setEnabled(true);
        }
    });
}

CFileForm::~CFileForm()
{
    delete ui;
}

QAbstractItemView *CFileForm::getCurrentItemView()
{
    if(mode == FileItemMode::TREE) {
        return ui->fileTreeView;
    } else {
        return ui->fileListView;
    }
}

void CFileForm::setViewMode(FileItemMode mode)
{
    this->mode = mode;
    switch (mode) {
    case FileItemMode::GRID: {
        ui->viewStackedWidget->setCurrentIndex(1);
        ui->fileListView->setViewMode(QListView::IconMode);
        break;
    }
    case FileItemMode::TREE: {
        ui->viewStackedWidget->setCurrentIndex(0);
        break;
    }
    default: {
        ui->viewStackedWidget->setCurrentIndex(1);
        ui->fileListView->setViewMode(QListView::ListMode);
        break;
    }
    }
}

FileItemMode CFileForm::viewMode()
{
    return mode;
}
