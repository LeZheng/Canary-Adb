#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QDebug>
#include <QFileDialog>
#include <QSettings>

#include "candroiddevice.h"

#if defined(Q_OS_WIN32)
const QString adbFileName = "adb.exe";
#elif defined(Q_OS_LINUX)
const QString adbFileName = "adb";
#elif defined(Q_OS_MACOS)
const QString adbFileName = "adb";
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("LeZheng");
    QCoreApplication::setOrganizationDomain("lezheng.com");
    QCoreApplication::setApplicationName("Canary adb");

    QTranslator translator;
    translator.load("canary_zh_CN.qm");
    a.installTranslator(&translator);

    QString defaultToolPath = QCoreApplication::applicationDirPath() + QDir::separator() + "platform-tools";

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings setting;
    QString adbPath = setting.value("anroid/adb/path",defaultToolPath + QDir::separator() + adbFileName).toString();
    while(!QFile(adbPath).exists()) {
        QString sdkPath = QFileDialog::getExistingDirectory(nullptr, a.tr("Choose Android SDK Directory"),
                          QDir::rootPath(),
                          QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(sdkPath.isEmpty() || !QDir(sdkPath).exists()) {
            return 0;
        } else {
            if(sdkPath.endsWith("platform-tools")) {
                defaultToolPath = sdkPath;
            } else {
                defaultToolPath = sdkPath + QDir::separator() + "platform-tools";
            }
            adbPath = defaultToolPath + QDir::separator() + adbFileName;
        }
    }
    setting.setValue("anroid/adb/path",adbPath);
    CAndroidContext::androidAdbPath = adbPath;

    MainWindow *window = new MainWindow;
    window->move((a.desktop()->width() - window->width()) / 2, (a.desktop()->height() - window->height()) / 2);

    QPixmap pixmap(":/img/startup_bg");
    QSplashScreen *splash = new QSplashScreen(pixmap.scaled(400,270));
    splash->resize(400,270);
    splash->show();
    splash->showMessage(splash->tr("start init..."),Qt::AlignBottom,Qt::white);
    a.processEvents();
    CAndroidContext::getInstance()->startListenAdb();
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::updateStateChanged,splash,[splash](const QString &msg) {
        splash->showMessage(msg,Qt::AlignBottom,Qt::white);
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::findNewDevice,splash,[splash](CAndroidDevice *device) {
        splash->showMessage(splash->tr("init device:%1 ...").arg(device->serialNumber),Qt::AlignBottom,Qt::white);
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListUpdated,splash,[splash,window]() {
        window->show();
        splash->finish(window);
        delete splash;
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListNotUpdated,splash,[splash,window]() {
        window->show();
        splash->finish(window);
        delete splash;
    });

    return a.exec();
}
