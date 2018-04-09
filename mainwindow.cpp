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
}

MainWindow::~MainWindow()
{
    delete ui;
}
