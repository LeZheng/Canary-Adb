#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include "candroiddevice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    CAndroidContext::getInstance()->startListenAdb();

    return a.exec();
}
