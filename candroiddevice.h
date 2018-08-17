#ifndef CANDROIDDEVICE_H
#define CANDROIDDEVICE_H

#include <QObject>
#include <QProcess>
#include <QMap>
#include <QTextCodec>
#include <QtConcurrent/QtConcurrent>

namespace candroid
{
class CAndroidApp;
class CAndroidDevice;
class CAndroidConfig;
class CAndroidFile;
}

class CAndroidDevice;

class ProcessResult
{
public:
    const int exitCode;
    const QString resultStr;
    const QByteArray resultArray;
    explicit ProcessResult(int exitCode,QString resultStr);
    explicit ProcessResult(int exitCode,QByteArray resultArr,QString resultStr = "");
};

class CAndroidFile
{

public:
    explicit CAndroidFile(const QString &basePath,const QString &info,CAndroidDevice *device);
    QString path;
    QString fileName;
    QString privilege;
    QString owner;
    QString group;
    QString size;
    QString time;
    const CAndroidDevice * device;
};

class CAndroidApp : public QObject
{
    Q_OBJECT

public:
    explicit CAndroidApp(const QString package,CAndroidDevice *parent = nullptr);
    const QString package;
    const CAndroidDevice * device;

    QString getName();
    ProcessResult uninstall();//(method "卸载应用" "adb uninstall com.qihoo360.mobilesafe")
    ProcessResult pmClear();//(method "清除应用数据" "adb shell pm clear com.qihoo360.mobilesafe")
    ProcessResult getRunningService();//(method "查看正在运行的 Services" "adb shell dumpsys activity services com.qihoo360.mobilesafe")
    ProcessResult getInfo();//(method "获取应用信息" "adb shell dumpsys package <packagename>")
    ProcessResult getInstallPath();//(method "获取安装路径" "adb shell pm path ecarx.weather")
    //TODO 与应用交互

private:
    QString name;
};

class CAndroidDevice : public QObject
{
    Q_OBJECT

public:
    explicit CAndroidDevice(const QString serialNumber,QObject *parent = nullptr);
    const QString serialNumber;
    QString getModel();
    void updateModel();
    QString getBattery();
    void updateBattery();
    QSize getWmSize();
    void updateWmSize();
    void setWmSize(int width,int height);
    void resetWmSize();
    QString getWmDensity();
    void updateWmDensity();
    void setWmDensity(int density);
    void resetWmDensity();
    QString getWindowDisplay();
    void updateWindowDiaplay();
    QString getAndroidId();
    void updateAndroidId();
    QString getAndroidVersion();
    void updateAndroidVersion();
    QString getNetworkInfo();
    void updateNetworkInfo();
    QString getCpuInfo();
    void updateCpuInfo();
    QString getMemInfo();
    void updateMemInfo();
    QMap<QString,QString> getSystemProps();
    QString getSystemProp(QString key);
    void updateSystemProps();
    ProcessResult screenShot(QString path);// (method "屏幕截图" "adb shell screencap -p /sdcard/sc.png")
    ProcessResult screenShot();
    ProcessResult pull(QString srcPath,QString desPath = ".");// (method "导出文件或目录" "adb pull /sdcard/sc.png")
    ProcessResult push(QString srcPath,QString desPath);// (method "导入文件或目录" "adb push ~/sr.mp4 /sdcard/")
    QProcess * screenRecord(QString recordPath,QString size = "",int bitRate = 4);// (method "屏幕录制" "adb shell screenrecord /sdcard/filename.mp4 --time-limit 5")
    ProcessResult reboot();// (method "重启手机" "adb reboot")
    void updatePackageList();// (method "获取应用列表" "adb shell pm list packages"
    QList<CAndroidApp *> getApplications();
    ProcessResult install(QString apkPath);// (method "安装应用" "adb install -rsd temp.apk"
    ProcessResult getRunningService();// (method "查看正在运行的 Services" "adb shell dumpsys activity services")
    //TODO 按键模拟和输入
    ProcessResult inputKeyEvent(int keyCode);
    ProcessResult inputSwipe(const QPoint &startPos,const QPoint &endPos,int duration);
    ProcessResult inputClick(const QPoint &pos);
    QProcess * logcat(QString format = "",QString patterns = "",QString tag = "",QString content = "",QString pid = 0);// (method "查看日志" "adb logcat -v long ActivityManager:I *:S")
    QList<CAndroidFile> listDir(const QString &path);
private:
    QString model;//(field "型号" :read "adb shell getprop ro.product.model")
    QString battery;//(field "电池状况" :read "adb shell dumpsys battery")
    QSize wmSize;//(field "屏幕分辨率" :read "adb shell wm size" :write "adb shell wm size 480x1024" :reset "adb shell wm size reset")
    QString wmDensity;//(field "屏幕密度" :read "adb shell wm density" :write "adb shell wm density 160" :reset "adb shell wm density reset")
    QString windownDisplay;//(field "显示器参数" :read "adb shell dumpsys window displays")
    QString androidId;//(field "android-id" :read "adb shell settings get secure android_id")
    QString androidVersion;// (field "android-version" :read "adb shell getprop ro.build.version.release")
    QString networkInfo;// (field "网络信息" :read "adb shell ifconfig")
    QString cpuInfo;// (field "cpu 信息" :read "adb shell cat /proc/cpuinfo")
    QString memInfo;// (field "内存信息" :read "adb shell cat /proc/meminfo")
    QMap<QString, QString> sysProps;// (field "系统属性" :read "adb shell cat /system/build.prop")
    QMap<QString,CAndroidApp *> applicationMap;
};

class CAndroidContext : public QObject
{
    Q_OBJECT
public:
    static CAndroidContext * config;
    static QString androidSdkPath;
    static QString androidAdbPath;
    static QList<CAndroidDevice *> getDevices();
    static CAndroidDevice * getDevice(const QString &serialNumber);
    static CAndroidContext * getInstance()
    {
        return config;
    }
    void startListenAdb();
    void stopListenAdb();

signals:
    void deviceListUpdated();
    void updateStateChanged(const QString &msg);
    void findNewDevice(CAndroidDevice *device);
    void deviceListNotUpdated();

private:
    QObjectCleanupHandler cleanupHandler;
    QMutex deviceMapMutex;
    volatile bool isRunning = false;
    QMap<QString,CAndroidDevice *> deviceMap;
    CAndroidContext() {}

};

#endif // CANDROIDDEVICE_H
