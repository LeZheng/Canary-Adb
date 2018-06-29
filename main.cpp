#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QDebug>

#include "candroiddevice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow *window = new MainWindow;
    window->move((a.desktop()->width() - window->width()) / 2, (a.desktop()->height() - window->height()) / 2);

    QPixmap pixmap(":/img/startup_bg");
    QSplashScreen *splash = new QSplashScreen(pixmap.scaled(400,270));
    splash->resize(400,270);
    splash->show();
    splash->showMessage("start init...",Qt::AlignBottom,Qt::white);
    a.processEvents();
    CAndroidContext::getInstance()->startListenAdb();
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::updateStateChanged,splash,[splash](const QString &msg){
        splash->showMessage(msg,Qt::AlignBottom,Qt::white);
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::findNewDevice,splash,[splash](CAndroidDevice *device){
        splash->showMessage(splash->tr("init device:%1 ...").arg(device->serialNumber),Qt::AlignBottom,Qt::white);
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListUpdated,splash,[splash,window](){
        window->show();
        splash->finish(window);
        delete splash;
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListNotUpdated,splash,[splash,window](){
        window->show();
        splash->finish(window);
        delete splash;
    });

    return a.exec();
}
