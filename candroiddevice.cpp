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

    QByteArray array = process.readAll();
    QString resultStr = QTextCodec::codecForLocale()->toUnicode(array);
    return ProcessResult(process.exitCode(),array,resultStr);
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
    this->model = processCmd(QString("%1 -s %2 shell getprop ro.product.model")
                             .arg(CAndroidContext::androidAdbPath)
                             .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getBattery()
{
    return this->battery;
}

void CAndroidDevice::updateBattery()
{
    this->battery = processCmd(QString("%1 -s %2 shell dumpsys battery")
                               .arg(CAndroidContext::androidAdbPath)
                               .arg(serialNumber)).resultStr.trimmed();
}

QSize CAndroidDevice::getWmSize()
{
    return this->wmSize;
}

void CAndroidDevice::updateWmSize()
{
    QString wmSizeStr = processCmd(QString("%1 -s %2 shell wm size")
                                   .arg(CAndroidContext::androidAdbPath)
                                   .arg(serialNumber)).resultStr.trimmed();

    wmSizeStr = wmSizeStr.right(wmSizeStr.size() - wmSizeStr.lastIndexOf(':') - 1).trimmed();
    if(wmSizeStr.contains('x')) {
        int width,height;
        int xIndex = wmSizeStr.indexOf('x');
        bool ok;
        width = wmSizeStr.left(xIndex).toInt(&ok);
        if(ok) {
            height = wmSizeStr.right(wmSizeStr.size() - xIndex - 1).toInt(&ok);
        }
        if(ok) {
            this->wmSize.setHeight(height);
            this->wmSize.setWidth(width);
        }
    }
}

void CAndroidDevice::setWmSize(int width, int height)
{
    ProcessResult result = processCmd(QString("%1 -s %2 shell wm size %2x%3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(width)
                                      .arg(height));
}

void CAndroidDevice::resetWmSize()
{
    ProcessResult result = processCmd(QString("%1 -s %2 shell wm size reset")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));
}

QString CAndroidDevice::getWmDensity()
{
    return this->wmDensity;
}

void CAndroidDevice::updateWmDensity()
{
    this->wmDensity = processCmd(QString("%1 -s %2 shell wm density")
                                 .arg(CAndroidContext::androidAdbPath)
                                 .arg(serialNumber)).resultStr.trimmed();
}

void CAndroidDevice::setWmDensity(int density)
{
    ProcessResult result = processCmd(QString("%1 -s %2 shell wm density %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(density));
}

void CAndroidDevice::resetWmDensity()
{
    ProcessResult result = processCmd(QString("%1 -s %2 shell wm density reset")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));
}

QString CAndroidDevice::getWindowDisplay()
{
    return this->windownDisplay;
}

void CAndroidDevice::updateWindowDiaplay()
{
    this->windownDisplay = processCmd(QString("%1 -s %2 shell dumpsys window displays")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getAndroidId()
{
    return this->androidId;
}

void CAndroidDevice::updateAndroidId()
{
    this->androidId = processCmd(QString("%1 -s %2 shell settings get secure android_id")
                                 .arg(CAndroidContext::androidAdbPath)
                                 .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getAndroidVersion()
{
    return this->androidVersion;
}

void CAndroidDevice::updateAndroidVersion()
{
    this->androidVersion = processCmd(QString("%1 -s %2 shell getprop ro.build.version.release")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getNetworkInfo()
{
    return this->networkInfo;
}

void CAndroidDevice::updateNetworkInfo()
{
    this->networkInfo = processCmd(QString("%1 -s %2 shell ifconfig")
                                   .arg(CAndroidContext::androidAdbPath)
                                   .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getCpuInfo()
{
    return this->cpuInfo;
}

void CAndroidDevice::updateCpuInfo()
{
    this->cpuInfo = processCmd(QString("%1 -s %2 shell cat /proc/cpuinfo")
                               .arg(CAndroidContext::androidAdbPath)
                               .arg(serialNumber)).resultStr.trimmed();
}

QString CAndroidDevice::getMemInfo()
{

    return this->memInfo;
}

void CAndroidDevice::updateMemInfo()
{
    this->memInfo = processCmd(QString("%1 -s %2 shell cat /proc/meminfo")
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
    ProcessResult result = processCmd(QString("%1 -s %2 shell cat /system/build.prop")
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

ProcessResult CAndroidDevice::screenShot(QString path)
{
    ProcessResult result = processCmd(QString("%1 -s %2 shell screencap -p %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(path));
    return result;
}

ProcessResult CAndroidDevice::screenShot()
{
    ProcessResult result = processCmd(QString("%1 -s %2 shell screencap -p")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));
    return result;
}

ProcessResult CAndroidDevice::pull(QString srcPath, QString desPath)
{
    return processCmd(QString("%1 -s %2 pull %3 %4")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(serialNumber)
                      .arg(srcPath)
                      .arg(desPath));
}

ProcessResult CAndroidDevice::push(QString srcPath, QString desPath)
{
    return processCmd(QString("%1 -s %2 push %3 %4")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(serialNumber)
                      .arg(srcPath)
                      .arg(desPath));
}

QProcess * CAndroidDevice::screenRecord(QString recordPath,QString size,int bitRate)
{
    QString cmd = QString("%1 -s %2 shell screenrecord ")
                  .arg(CAndroidContext::androidAdbPath)
                  .arg(serialNumber);
    if(!size.isEmpty()) {
        cmd.append(QString(" --size %1 ").arg(size));
    }
    if(bitRate > 0) {
        cmd.append(QString(" --bit-rate %1 ").arg(bitRate));
    }
    cmd.append(recordPath);

    QProcess * process = new QProcess();
    process->start(cmd);
    return process;
}

ProcessResult CAndroidDevice::reboot()
{
    return processCmd(QString("%1 -s %2 reboot")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(serialNumber));
}

void CAndroidDevice::updatePackageList()
{
    ProcessResult result = processCmd(QString("%1 -s %2 shell pm list packages")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber));
    QListIterator<QString> packageIter(result.resultStr.split('\n'));
    const QString packagePrefix = "package:";
    while(packageIter.hasNext()) {
        QString packageName = packageIter.next();
        if(!packageName.isEmpty()) {
            if(packageName.startsWith(packagePrefix)) {
                packageName = packageName.right(packageName.size() - packagePrefix.size());
                applicationMap.insert(packageName,new CAndroidApp(packageName,this));
            } else {
                applicationMap.insert(packageName,new CAndroidApp(packageName,this));
            }
        }
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

ProcessResult CAndroidDevice::install(QString apkPath)
{
    return processCmd(QString("%1 -s %2 install -r %3")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(serialNumber)
                      .arg(apkPath));
}

ProcessResult CAndroidDevice::getRunningService()
{
    return processCmd(QString("%1 -s %2 shell dumpsys activity services")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(serialNumber));
}

ProcessResult CAndroidDevice::inputKeyEvent(int keyCode)
{
    return processCmd(QString("%1 -s %2 shell input keyevent %3")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(serialNumber)
                      .arg(keyCode));
}

ProcessResult CAndroidDevice::inputSwipe(const QPoint &startPos, const QPoint &endPos, int duration)
{
    return processCmd(QString("%1 -s %2 shell input swipe %3 %4 %5 %6 %7")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(serialNumber)
                      .arg(startPos.x()).arg(startPos.y()).arg(endPos.x()).arg(endPos.y()).arg(duration));
}

ProcessResult CAndroidDevice::inputClick(const QPoint &pos)
{
    return processCmd(QString("%1 -s %2 shell input tap %3 %4")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(serialNumber)
                      .arg(pos.x()).arg(pos.y()));
}

QProcess *CAndroidDevice::logcat(QString format, QString logLevel, QString tag, QString content, QString pid)
{
    QString cmd = QString("%1 -s %2 logcat")
                  .arg(CAndroidContext::androidAdbPath)
                  .arg(serialNumber);
    if(!format.isEmpty())
        cmd.append(" -v ").append(format);
    if(tag.isEmpty())
        tag = "*";
    if(logLevel.size() > 1)
        logLevel = logLevel.left(1);
    if(!logLevel.isEmpty())
        cmd.append(QString(" -s \"%1:%2\"").arg(tag).arg(logLevel));
    if(!content.isEmpty())
        cmd.append(" -e \"").append(content).append("\"");
    if(!pid.isEmpty())
        cmd.append(" --pid ").append(pid);

    qDebug() << cmd;
    QProcess * process = new QProcess();
    process->start(cmd);
    return process;
}

QList<CAndroidFile> CAndroidDevice::listDir(const QString &path)
{
    QList<CAndroidFile> fileList;
    ProcessResult result = processCmd(QString("%1 -s %2 shell ls -l %3")
                                      .arg(CAndroidContext::androidAdbPath)
                                      .arg(serialNumber)
                                      .arg(path));
    QStringList lines = result.resultStr.split("\n");
    QListIterator<QString> iterator(lines);
    while(iterator.hasNext()) {
        QString line = iterator.next();
        if(line.startsWith('d') || line.startsWith('-')) {
            fileList.append(CAndroidFile(path,line.trimmed(),this));
        }
    }
    return fileList;
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

CAndroidDevice *CAndroidContext::getDevice(const QString &serialNumber)
{
    if(CAndroidContext::getInstance()->deviceMap.contains(serialNumber)) {
        return CAndroidContext::getInstance()->deviceMap.value(serialNumber);
    } else {
        return nullptr;
    }
}

void CAndroidContext::startListenAdb()
{
    if(!isRunning) {
        isRunning = true;
        QtConcurrent::run([this]() {
            while(this->isRunning) {
                emit updateStateChanged(tr("update devices"));
                ProcessResult result = processCmd(QString("%1 devices").arg(CAndroidContext::androidAdbPath));
                if(result.exitCode == 0) {
                    QStringList deviceStrList = result.resultStr.trimmed().split("\n");
                    if(deviceStrList.size() > 0) {
                        deviceStrList.removeAt(0);
                    }
                    this->deviceMapMutex.lock();
                    QMap<QString,CAndroidDevice *> tmpDeviceMap;
                    for(int i = 0; i < deviceStrList.size(); i++) {
                        QString line = deviceStrList.at(i);
                        line = line.trimmed();
                        if(line.endsWith("device")) {
                            QString serialNumber = line.left(line.length() - 6).trimmed();
                            if(this->deviceMap.contains(serialNumber)) {
                                CAndroidDevice * tmpDevice = this->deviceMap.value(serialNumber);
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
                                emit updateStateChanged(tr("udpate device[%1]'s package list").arg(serialNumber));
                                device->updatePackageList();
                                tmpDeviceMap.insert(serialNumber,device);
                            }
                        }
                    }
                    bool needUpdate = (tmpDeviceMap.size() != this->deviceMap.size());
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
    return package;
}

ProcessResult CAndroidApp::uninstall()
{
    return processCmd(QString("%1 -s %2 uninstall %3")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(device->serialNumber)
                      .arg(package));
}

ProcessResult CAndroidApp::pmClear()
{
    return processCmd(QString("%1 -s %2 shell pm clear %3")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(device->serialNumber)
                      .arg(package));
}

ProcessResult CAndroidApp::getRunningService()
{
    return processCmd(QString("%1 -s %2 shell dumpsys activity services %3")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(device->serialNumber)
                      .arg(package));
}

ProcessResult CAndroidApp::getInfo()
{
    return processCmd(QString("%1 -s %2 shell dumpsys package %3")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(device->serialNumber)
                      .arg(package));
}

ProcessResult CAndroidApp::getInstallPath()
{
    return processCmd(QString("%1 -s %2 shell pm path %3")
                      .arg(CAndroidContext::androidAdbPath)
                      .arg(device->serialNumber)
                      .arg(package));
}

ProcessResult::ProcessResult(int exitCode, QString resultStr):
    exitCode(exitCode),resultStr(resultStr)
{

}

ProcessResult::ProcessResult(int exitCode, QByteArray resultArr,QString resultStr):
    exitCode(exitCode),resultArray(resultArr),resultStr(resultStr)
{

}

CAndroidFile::CAndroidFile(const QString &basePath,const QString &info, CAndroidDevice *device):
    device(device)
{
    if(info.isEmpty()) {
        this->fileName = device->tr("unnamed");
        this->path = device->tr("unknown");
        this->owner = device->tr("unknown");
        this->group = device->tr("unknown");
        this->size = device->tr("unknown");
        this->privilege = device->tr("unknown");
        this->time = device->tr("unknown");
    } else {
        QStringList infoList = info.split(QRegExp("\\s+"));
        if(infoList.size() >= 8) {
            infoList.removeAt(1);
        }
        if(infoList.size() >= 7) {
            this->size = infoList.at(3);
            infoList.removeAt(3);
        } else {
            this->size = "0";
        }
        if(infoList.size() >= 6) {
            int index = info.indexOf(infoList.at(4));
            this->fileName = info.right(info.size() - index - infoList.at(4).size()).trimmed();
            this->time = infoList.at(3) + " " + infoList.at(4);
            this->privilege = infoList.at(0);
            this->owner = infoList.at(1);
            this->group = infoList.at(2);
            this->path = basePath + "/" + this->fileName;
        }
    }
}
