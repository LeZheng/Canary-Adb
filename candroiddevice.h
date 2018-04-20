#ifndef CANDROIDDEVICE_H
#define CANDROIDDEVICE_H

#include <QObject>
#include <QProcess>
#include <QMap>
#include <QTextCodec>

namespace candroid {
class CAndroidApp;
class CAndroidDevice;
class CAndroidConfig;
}

class ProcessResult
{
public:
    const int exitCode;
    const QString resultStr;
    explicit ProcessResult(int exitCode,QString resultStr);
};

class CAndroidApp : public QObject
{
    Q_OBJECT

public:
    explicit CAndroidApp(const QString package,QObject *parent = nullptr);
    const QString package;

    QString getName();
    void uninstall();//(method "卸载应用" "adb uninstall com.qihoo360.mobilesafe")
    void pmClear();//(method "清除应用数据" "adb shell pm clear com.qihoo360.mobilesafe")
    QString getRunningService();//(method "查看正在运行的 Services" "adb shell dumpsys activity services com.qihoo360.mobilesafe")
    QString getInfo();//(method "获取应用信息" "adb shell dumpsys package <packagename>")
    QString getInstallPath();//(method "获取安装路径" "adb shell pm path ecarx.weather")
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
    QString getBattery();
    QString getWmSize();
    void setWmSize(int width,int height);
    void resetWmSize();
    QString getWmDensity();
    void setWmDensity(int density);
    void resetWmDensity();
    QString getWindownDisplay();
    QString getAndroidId();
    QString getAndroidVersion();
    QString getNetworkInfo();
    QString getCpuInfo();
    QString getMenInfo();
    QMap<QString,QString> getSystemProps();
    QString getSystemProp(QString key);
    void screenShot(QString path);// (method "屏幕截图" "adb shell screencap -p /sdcard/sc.png")
    void pull(QString srcPath,QString desPath = ".");// (method "导出文件或目录" "adb pull /sdcard/sc.png")
    void push(QString srcPath,QString desPath);// (method "导入文件或目录" "adb push ~/sr.mp4 /sdcard/")
    QProcess * screenRecord(QString recordPath,int timeLimit = 5);// (method "屏幕录制" "adb shell screenrecord /sdcard/filename.mp4 --time-limit 5")
    void reboot();// (method "重启手机" "adb reboot")
    QStringList pmListPackage();// (method "获取应用列表" "adb shell pm list packages"
    QList<CAndroidApp *> getApplications();
    void install(QString apkPath);// (method "安装应用" "adb install -rsd temp.apk"
    QString getRunningService();// (method "查看正在运行的 Services" "adb shell dumpsys activity services")
        //TODO 按键模拟和输入
    QProcess * logcat(QString patterns);// (method "查看日志" "adb logcat -v long ActivityManager:I *:S")

private:
    QString model;//(field "型号" :read "adb shell getprop ro.product.model")
    QString battery;//(field "电池状况" :read "adb shell dumpsys battery")
    QString wmSize;//(field "屏幕分辨率" :read "adb shell wm size" :write "adb shell wm size 480x1024" :reset "adb shell wm size reset")
    QString wmDensity;//(field "屏幕密度" :read "adb shell wm density" :write "adb shell wm density 160" :reset "adb shell wm density reset")
    QString windownDisplay;//(field "显示器参数" :read "adb shell dumpsys window displays")
    QString androidId;//(field "android-id" :read "adb shell settings get secure android_id")
    QString androidVersion;// (field "android-version" :read "adb shell getprop ro.build.version.release")
    QString networkInfo;// (field "网络信息" :read "adb shell ifconfig")
    QString cpuInfo;// (field "cpu 信息" :read "adb shell cat /proc/cpuinfo")
    QString memInfo;// (field "内存信息" :read "adb shell cat /proc/meminfo")
    QMap<QString, QString> sysProps;// (field "系统属性" :read "adb shell cat /system/build.prop")
};

class CAndroidConfig
{
public:
    static CAndroidConfig * config;
    static QString androidSdkPath;
    static QString androidAdbPath;
    static QList<CAndroidDevice *> getDevices();
    static CAndroidConfig * getInstance(){ return config; }

private:
    QList<CAndroidDevice *> deviceList;
    CAndroidConfig(){}

};

#endif // CANDROIDDEVICE_H
