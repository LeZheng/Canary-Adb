#include "candroiddevice.h"
#include <QDebug>

QString CAndroidContext::androidAdbPath = "/home/skyline/public/Android/Sdk/platform-tools/adb";
QString CAndroidContext::androidSdkPath = "";
QList<CAndroidDevice *> deviceList = QList<CAndroidDevice *>();
CAndroidContext * CAndroidContext::config = new CAndroidContext();

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
    return this->model;
}

void CAndroidDevice::updateModel()
{
    this->model = processCmd(tr("%1 -s %2 shell getprop ro.product.model")
                             .arg(CAndroidContext::androidAdbPath)
                             .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getBattery()
{
    return this->battery;
}

void CAndroidDevice::updateBattery()
{
    this->battery = processCmd(tr("%1 -s %2 shell dumpsys battery")
                               .arg(CAndroidContext::androidAdbPath)
                               .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getWmSize()
{
    return this->wmSize;
}

void CAndroidDevice::updateWmSize()
{
    this->wmSize = processCmd(tr("%1 -s %2 shell wm size")
                              .arg(CAndroidContext::androidAdbPath)
                              .arg(serialNumber)).resultStr.trimmed();
}

void CAndroidDevice::setWmSize(int width, int height)
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell wm size %2x%3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(width)
                                      .arg(height));
}

void CAndroidDevice::resetWmSize()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell wm size reset")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));
}

QString CAndroidDevice::getWmDensity()
{
    return this->wmDensity;
}

void CAndroidDevice::updateWmDensity()
{
    this->wmDensity = processCmd(tr("%1 -s %2 shell wm density")
                                 .arg(CAndroidContext::androidAdbPath)
                                 .arg(serialNumber)).resultStr.trimmed();
}

void CAndroidDevice::setWmDensity(int density)
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell wm density %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(density));
}

void CAndroidDevice::resetWmDensity()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell wm density reset")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));
}

QString CAndroidDevice::getWindowDisplay()
{
    return this->windownDisplay;
}

void CAndroidDevice::updateWindowDiaplay()
{
    this->windownDisplay = processCmd(tr("%1 -s %2 shell dumpsys window displays")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getAndroidId()
{
    return this->androidId;
}

void CAndroidDevice::updateAndroidId()
{
    this->androidId = processCmd(tr("%1 -s %2 shell settings get secure android_id")
                                 .arg(CAndroidContext::androidAdbPath)
                                 .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getAndroidVersion()
{
    return this->androidVersion;
}

void CAndroidDevice::updateAndroidVersion()
{
    this->androidVersion = processCmd(tr("%1 -s %2 shell getprop ro.build.version.release")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getNetworkInfo()
{
    return this->networkInfo;
}

void CAndroidDevice::updateNetworkInfo()
{
    this->networkInfo = processCmd(tr("%1 -s %2 shell ifconfig")
                                   .arg(CAndroidContext::androidAdbPath)
                                   .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getCpuInfo()
{
    return this->cpuInfo;
}

void CAndroidDevice::updateCpuInfo()
{
    this->cpuInfo = processCmd(tr("%1 -s %2 shell cat /proc/cpuinfo")
                               .arg(CAndroidContext::androidAdbPath)
                               .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getMemInfo()
{

    return this->memInfo;
}

void CAndroidDevice::updateMemInfo()
{
    this->memInfo = processCmd(tr("%1 -s %2 shell cat /proc/meminfo")
                               .arg(CAndroidContext::androidAdbPath)
                               .arg(serialNumber)).resultStr.trimmed();
}

QMap<QString, QString> CAndroidDevice::getSystemProps()
{
    return sysProps;
}

QString CAndroidDevice::getSystemProp(QString key)
{
    if(sysProps.contains(key))
        return sysProps[key];
    else
        return "";
}

void CAndroidDevice::updateSystemProps()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell cat /system/build.prop")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));

    QStringList lines = result.resultStr.split("\n");
    for(int i = 0; i < lines.length(); i++) {
        QString line = lines.at(i);
        QStringList keyAndValue = line.split('=');
        if(keyAndValue.size() == 2) {
            sysProps[keyAndValue.at(0)] = keyAndValue.at(1);
        }
    }
}

void CAndroidDevice::screenShot(QString path)
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell screencap -p %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(path));
}

void  CAndroidDevice::pull(QString srcPath, QString desPath)
{
    ProcessResult result = processCmd(tr("%1 -s %2 pull %3 %4")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(srcPath)
                                      .arg(desPath));
}

void  CAndroidDevice::push(QString srcPath, QString desPath)
{
    ProcessResult result = processCmd(tr("%1 -s %2 push %3 %4")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(srcPath)
                                      .arg(desPath));
}

QProcess * CAndroidDevice::screenRecord(QString recordPath, int timeLimit)
{
    QProcess * process = new QProcess(this);
    process->start(tr("%1 -s %2 shell screenrecord %3 --time-limit %4")
                   .arg(CAndroidContext::androidAdbPath)
                   .arg(serialNumber)
                   .arg(recordPath)
                   .arg(timeLimit));
    return process;//TODO
}

void CAndroidDevice::reboot()
{
    ProcessResult result = processCmd(tr("%1 -s %2 reboot")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));
}

void CAndroidDevice::pmListPackage()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell pm list packages")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));
    QListIterator<QString> packageIter(result.resultStr.split('\n'));
    while(packageIter.hasNext()) {
        QString packageName = packageIter.next();
        applicationMap.insert(packageName,new CAndroidApp(packageName,this));
    }
}

QList<CAndroidApp *> CAndroidDevice::getApplications()
{
    QList<CAndroidApp *> applicationList;
    QMapIterator<QString,CAndroidApp *> iter(this->applicationMap);
    while(iter.hasNext()) {
        applicationList << iter.next().value();
    }
    return applicationList;
}

void CAndroidDevice::install(QString apkPath)
{
    ProcessResult result = processCmd(tr("%1 -s %2 install -r %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(apkPath));
}

QString CAndroidDevice::getRunningService()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell dumpsys activity services")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));

    return result.resultStr;
}

void CAndroidDevice::inputKeyEvent(int keyCode)
{
    processCmd(tr("%1 -s %2 shell input keyevent %3")
               .arg(CAndroidContext::androidAdbPath)
               .arg(serialNumber)
               .arg(keyCode));
}

QProcess *CAndroidDevice::logcat(QString format, QString logLevel, QString tag, QString content, QString pid)
{
    QString cmd = tr("%1 -s %2 logcat")
                  .arg(CAndroidContext::androidAdbPath)
                  .arg(serialNumber);
    if(!format.isEmpty())
        cmd.append(" -v ").append(format);
    if(tag.isEmpty())
        tag = "*";
    if(logLevel.size() > 1)
        logLevel = logLevel.left(1);
    if(!logLevel.isEmpty())
        cmd.append(tr(" -s \"%1:%2\"").arg(tag).arg(logLevel));
    if(!content.isEmpty())
        cmd.append(" -e \"").append(content).append("\"");
    if(!pid.isEmpty())
        cmd.append(" --pid ").append(pid);

    qDebug() << cmd;
    QProcess * process = new QProcess();
    process->start(cmd);
    return process;
}

QList<CAndroidDevice *> CAndroidContext::getDevices()
{
    QMutexLocker locker(&(CAndroidContext::getInstance()->deviceMapMutex));
    QList<CAndroidDevice *> deviceList;
    QMapIterator<QString,CAndroidDevice *> iter(CAndroidContext::getInstance()->deviceMap);
    while(iter.hasNext()) {
        deviceList << iter.next().value();
    }
    return deviceList;
}

void CAndroidContext::startListenAdb()
{
    if(!isRunning) {
        isRunning = true;
        QtConcurrent::run([this]() {
            while(this->isRunning) {
                emit updateStateChanged(tr("update devices"));
                ProcessResult result = processCmd(tr("%1 devices").arg(CAndroidContext::androidAdbPath));
                if(result.exitCode == 0) {
                    QStringList deviceStrList = result.resultStr.trimmed().split("\n");
                    if(deviceStrList.size() > 0) {
                        deviceStrList.removeAt(0);
                    }
                    bool needUpdate = (deviceStrList.size() != this->deviceMap.size());
                    this->deviceMapMutex.lock();
                    QMap<QString,CAndroidDevice *> tmpDeviceMap;
                    for(int i = 0; i < deviceStrList.size(); i++) {
                        QString line = deviceStrList.at(i);
                        line = line.trimmed();
                        if(line.endsWith("device")) {
                            QString serialNumber = line.left(line.length() - 6).trimmed();
                            if(this->deviceMap.contains(serialNumber)) {
                                CAndroidDevice * tmpDevice = this->deviceMap.take(serialNumber);
                                tmpDeviceMap.insert(serialNumber,tmpDevice);
                                this->cleanupHandler.remove(tmpDevice);
                            } else {
                                CAndroidDevice * device = new CAndroidDevice(serialNumber);
                                emit findNewDevice(device);

                                emit updateStateChanged(tr("udpate device[%1]'s name").arg(serialNumber));
                                device->updateModel();
                                emit updateStateChanged(tr("udpate device[%1]'s wm density").arg(serialNumber));
                                device->updateWmDensity();
                                emit updateStateChanged(tr("udpate device[%1]'s wm size").arg(serialNumber));
                                device->updateWmSize();
                                emit updateStateChanged(tr("udpate device[%1]'s android id").arg(serialNumber));
                                device->updateAndroidId();
                                emit updateStateChanged(tr("udpate device[%1]'s android version").arg(serialNumber));
                                device->updateAndroidVersion();
                                tmpDeviceMap.insert(serialNumber,device);
                            }
                        }
                    }
                    this->deviceMap.clear();
                    this->deviceMap = tmpDeviceMap;
                    this->cleanupHandler.clear();
                    QMapIterator<QString,CAndroidDevice *> iter(this->deviceMap);
                    while(iter.hasNext()) {
                        this->cleanupHandler.add(iter.next().value());
                    }
                    this->deviceMapMutex.unlock();
                    if(needUpdate) {
                        emit deviceListUpdated();
                    } else {
                        emit deviceListNotUpdated();
                    }
                } else {
                    //TODO error
                }
                QThread::msleep(500);
            }
        });
    }
}

void CAndroidContext::stopListenAdb()
{
    isRunning = false;
}

CAndroidApp::CAndroidApp(QString package,CAndroidDevice *parent):
    QObject(parent),package(package),device(parent)
{

}

QString CAndroidApp::getName()
{
    return "";//TODO
}

void CAndroidApp::uninstall()
{
    ProcessResult result = processCmd(tr("%1 -s %2 uninstall %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(device->serialNumber)
                                      .arg(package));
}

void CAndroidApp::pmClear()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell pm clear %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(device->serialNumber)
                                      .arg(package));
}

QString CAndroidApp::getRunningService()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell dumpsys activity services %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(device->serialNumber)
                                      .arg(package));
    return result.resultStr;
}

QString CAndroidApp::getInfo()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell dumpsys package %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(device->serialNumber)
                                      .arg(package));
    return result.resultStr;
}

QString CAndroidApp::getInstallPath()
{
    ProcessResult result = processCmd(tr("%1 -s %2 shell pm path %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(device->serialNumber)
                                      .arg(package));
    return result.resultStr;
}

ProcessResult::ProcessResult(int exitCode, QString resultStr):
    exitCode(exitCode),resultStr(resultStr)
{

}
