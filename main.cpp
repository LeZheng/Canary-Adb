#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include <QDir>

#include "candroiddevice.h"
#include "cstartupform.h"

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

    QDir().mkdir("adb_log");

    QCoreApplication::setOrganizationName("LeZheng");
    QCoreApplication::setOrganizationDomain("lezheng.com");
    QCoreApplication::setApplicationName("Canary adb");

    QTranslator translator;
    translator.load("canary_zh_CN.qm");
    a.installTranslator(&translator);

    QString defaultToolPath = QCoreApplication::applicationDirPath() + QDir::separator() + "platform-tools";

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings setting(QCoreApplication::applicationDirPath() + QDir::separator() + "app-config.ini", QSettings::IniFormat);
    QString adbPath = setting.value("anroid/adb/path",defaultToolPath + QDir::separator() + adbFileName).toString();
    qDebug() << adbPath;
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
    window->setWindowTitle(QCoreApplication::applicationName());
    window->move((a.desktop()->width() - window->width()) / 2, (a.desktop()->height() - window->height()) / 2);

    QPixmap pixmap(":/img/startup_bg");

    CStartUpForm *splash = new CStartUpForm();
    splash->setAttribute(Qt::WA_DeleteOnClose,true);
    splash->move((a.desktop()->width() - splash->width()) / 2, (a.desktop()->height() - splash->height()) / 2);
    splash->show();
    splash->showMessage(splash->tr("start init..."),Qt::AlignBottom,Qt::yellow);
    a.processEvents();
    CAndroidContext::getInstance()->startListenAdb();
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::updateStateChanged,splash,[splash](int progress,const QString &msg) {
        splash->showMessage(msg,Qt::AlignBottom,Qt::yellow);
        splash->setCurrentProgress(progress);
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::findNewDevice,splash,[splash](CAndroidDevice *device) {
        splash->showMessage(splash->tr("init device:%1 ...").arg(device->serialNumber),Qt::AlignBottom,Qt::yellow);
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListUpdated,splash,[splash,window]() {
        window->show();
        splash->close();
    });
    a.connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListNotUpdated,splash,[splash,window]() {
        window->show();
        splash->close();
    });



    return a.exec();
}
