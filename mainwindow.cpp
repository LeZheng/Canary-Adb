#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ui->leftDockWidget->setTitleBarWidget(new QWidget(this));
    this->ui->leftDockWidget->titleBarWidget()->setFixedHeight(0);
    this->ui->rightDockWidget->setTitleBarWidget(new QWidget(this));
    this->ui->rightDockWidget->titleBarWidget()->setFixedHeight(0);
    this->ui->bottomDockWidget->setTitleBarWidget(new QWidget(this));
    this->ui->bottomDockWidget->titleBarWidget()->setFixedHeight(0);

    initToolBar();
    initFileWidget();
    initDeviceWidget();
    initConsoleWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBar()
{
    //TODO
}

void MainWindow::initFileWidget()
{
    ui->leftDockWidget->setWidget(new CFileForm(this));
    //TODO
}

void MainWindow::initDeviceWidget()
{
    ui->rightDockWidget->setWidget(new CDeviceForm(this));
    //TODO
}

void MainWindow::initConsoleWidget()
{
    ui->bottomDockWidget->setWidget(new CConsoleForm(this));
    //TODO
}
