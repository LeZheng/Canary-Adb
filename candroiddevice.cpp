#include "candroiddevice.h"
#include <QDebug>

QString CAndroidConfig::androidAdbPath = "/home/skyline/public/Android/Sdk/platform-tools/adb";
QString CAndroidConfig::androidSdkPath = "";
QList<CAndroidDevice *> deviceList = QList<CAndroidDevice *>();
CAndroidConfig * CAndroidConfig::config = new CAndroidConfig();

ProcessResult processCmd(QString cmd)
{
    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1);

    QString resultStr;
    resultStr.append(QTextCodec::codecForLocale()->toUnicode(process.readAll()));
    return ProcessResult(process.exitCode(),resultStr);
}



CAndroidDevice::CAndroidDevice(QString serialNumber,QObject *parent) :
    QObject(parent),serialNumber(serialNumber)
{

}

QString CAndroidDevice::getModel()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell getprop ro.product.model")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

QString CAndroidDevice::getBattery()
{
    ProcessResult result =processCmd(tr("%1 -s %2 shell dumpsys battery")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber));
    return result.resultStr;
}

QString CAndroidDevice::getWmSize()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell wm size")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

void CAndroidDevice::setWmSize(int width, int height)
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell wm size %2x%3")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(width)
                                      .arg(height));
}

void CAndroidDevice::resetWmSize()
{
    int exitCode = QProcess::execute(tr("%1 -s %2 shell wm size reset")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber));
}

QString CAndroidDevice::getWmDensity()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell wm density")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

void CAndroidDevice::setWmDensity(int density)
{
    int exitCode = QProcess::execute(tr("%1 -s %2 shell wm density %3")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber)
                                     .arg(density));
}

void CAndroidDevice::resetWmDensity()
{
    int exitCode = QProcess::execute(tr("%1 -s %2 shell wm density reset")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber));
}

QString CAndroidDevice::getWindownDisplay()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell dumpsys window displays")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

QString CAndroidDevice::getAndroidId()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell settings get secure android_id")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

QString CAndroidDevice::getAndroidVersion()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell getprop ro.build.version.release")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

QString CAndroidDevice::getNetworkInfo()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell ifconfig")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

QString CAndroidDevice::getCpuInfo()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell cat /proc/cpuinfo")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

QString CAndroidDevice::getMenInfo()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell cat /proc/meminfo")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    return result.resultStr;
}

QMap<QString, QString> CAndroidDevice::getSystemProps()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell cat /system/build.prop")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));

    QStringList lines = result.resultStr.split("\n");
    for(int i = 0;i < lines.length();i++)
    {
        QString line = lines.at(i);
        QStringList keyAndValue = line.split('=');
        if(keyAndValue.size() == 2)
        {
            sysProps[keyAndValue.at(0)] = keyAndValue.at(1);
        }
    }
    return sysProps;
}

QString CAndroidDevice::getSystemProp(QString key)
{
    if(sysProps.size() == 0){
        getSystemProps();
    }
    if(sysProps.contains(key))
        return sysProps[key];
    else
        return "";
}

void CAndroidDevice::screenShot(QString path)
{
    int exitCode = QProcess::execute(tr("%1 -s %2 shell screencap -p %3")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber)
                                     .arg(path));
}

void  CAndroidDevice::pull(QString srcPath, QString desPath)
{
    int exitCode = QProcess::execute(tr("%1 -s %2 pull %3 %4")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber)
                                     .arg(srcPath)
                                     .arg(desPath));
}

void  CAndroidDevice::push(QString srcPath, QString desPath)
{
    int exitCode = QProcess::execute(tr("%1 -s %2 push %3 %4")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber)
                                     .arg(srcPath)
                                     .arg(desPath));
}

QProcess * CAndroidDevice::screenRecord(QString recordPath, int timeLimit)
{
    QProcess * process = new QProcess(this);
    process->start(tr("%1 -s %2 shell screenrecord %3 --time-limit %4")
                   .arg(CAndroidConfig::androidAdbPath)
                   .arg(serialNumber)
                   .arg(recordPath)
                   .arg(timeLimit));
    return process;//TODO
}

void CAndroidDevice::reboot()
{
    int exitCode = QProcess::execute(tr("%1 -s %2 reboot")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber));
}

QStringList CAndroidDevice::pmListPackage()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell pm list packages")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));

    return result.resultStr.split('\n');
}

QList<CAndroidApp *> CAndroidDevice::getApplications()
{
    QList<CAndroidApp *> applicationList;
    ProcessResult result = processCmd(tr("%1 -s %2 shell pm list packages")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));
    QListIterator<QString> packageIter(result.resultStr.split('\n'));
    for(;packageIter.hasNext();)
    {
        applicationList << new CAndroidApp(packageIter.next(),this);
    }
    return applicationList;
}

void CAndroidDevice::install(QString apkPath)
{
    int exitCode = QProcess::execute(tr("%1 -s %2 install -rsd %3")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber)
                                     .arg(apkPath));
}

QString CAndroidDevice::getRunningService()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell dumpsys activity services")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber));

    return result.resultStr;
}

QProcess *CAndroidDevice::logcat(QString patterns)
{
    return nullptr;//TODO
}

QList<CAndroidDevice *> CAndroidConfig::getDevices()
{
    CAndroidConfig::getInstance()->deviceList.clear();
    QStringList deviceStrList;
    QString cmd = CAndroidConfig::androidAdbPath;
    ProcessResult result = processCmd(cmd.append(" devices"));
    deviceStrList = result.resultStr.split("\n");
    deviceStrList.removeAt(0);
    for(int i = 0;i < deviceStrList.size();i++)
    {
        QString line = deviceStrList.at(i);
        line = line.trimmed();
        if(line.endsWith("device"))
        {
            QString serialNumber = line.left(line.length() - 6).trimmed();
            CAndroidConfig::getInstance()->deviceList.append(new CAndroidDevice(serialNumber));
        }
    }
    return CAndroidConfig::getInstance()->deviceList;
}

CAndroidApp::CAndroidApp(QString package,QObject *parent):
    QObject(parent),package(package)
{

}

QString CAndroidApp::getName()
{
    return "";//TODO
}

void CAndroidApp::uninstall()
{
    int exitCode = QProcess::execute(tr("%1 -s %2 uninstall %3")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber)
                                     .arg(package));
}

void CAndroidApp::pmClear()
{
    int exitCode = QProcess::execute(tr("%1 -s %2 shell pm clear %3")
                                     .arg(CAndroidConfig::androidAdbPath)
                                     .arg(serialNumber)
                                     .arg(package));
}

QString CAndroidApp::getRunningService()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell dumpsys activity services %3")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(package));
    return result.resultStr;
}

QString CAndroidApp::getInfo()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell dumpsys package %3")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(package));
    return result.resultStr;
}

QString CAndroidApp::getInstallPath()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell pm path %3")
                                      .arg(CAndroidConfig::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(package));
    return result.resultStr;
}

ProcessResult::ProcessResult(int exitCode, QString resultStr):
    exitCode(exitCode),resultStr(resultStr)
{

}
