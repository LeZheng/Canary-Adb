#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    this->ui->leftDockWidget->setWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}
