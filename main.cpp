#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include "candroiddevice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QList<CAndroidDevice *> devices = CAndroidConfig::getDevices();
    for(int i = 0; i < devices.size(); i++) {
        qDebug() << devices.at(i)->serialNumber;
    }

    return a.exec();
}
