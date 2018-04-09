#include "candroiddevice.h"

CAndroidDevice::CAndroidDevice(QString serialNumber,QObject *parent) :
    QObject(parent),serialNumber(serialNumber)
{

}

QString CAndroidDevice::getModel()
{
    return "";//TODO
}

QString CAndroidDevice::getBattery()
{
    return "";//TODO
}

QString CAndroidDevice::getWmSize()
{
    return "";//TODO
}

void CAndroidDevice::setWmSize(int width, int height)
{
    //TODO
}

void CAndroidDevice::resetWmSize()
{
    //TODO
}

QString CAndroidDevice::getWmDensity()
{
    return "";//TODO
}

void CAndroidDevice::setWmDensity(int density)
{
    //TODO
}

void CAndroidDevice::resetWmDensity()
{
    //TODO
}

QString CAndroidDevice::getWindownDisplay()
{
    return "";//TODO
}

QString CAndroidDevice::getAndroidId()
{
    return "";//TODO
}

QString CAndroidDevice::getAndroidVersion()
{
    return "";//TODO
}

QString CAndroidDevice::getNetworkInfo()
{
    return "";//TODO
}

QString CAndroidDevice::getCpuInfo()
{
    return "";//TODO
}

QString CAndroidDevice::getMenInfo()
{
    return "";//TODO
}

QMap<QString, QString> CAndroidDevice::getSystemProps()
{
    return QMap<QString, QString>();//TODO
}

QString CAndroidDevice::getSystemProp(QString key)
{
    return "";//TODO
}

void CAndroidDevice::screenShot(QString path)
{
    //TODO
}

void  CAndroidDevice::pull(QString srcPath, QString desPath)
{
    //TODO
}

void  CAndroidDevice::push(QString srcPath, QString desPath)
{
    //TODO
}

QProcess * CAndroidDevice::screenRecord(QString recordPath, int timeLimit)
{
    return nullptr;//TODO
}

void CAndroidDevice::reboot()
{
    //TODO
}

QStringList CAndroidDevice::pmListPackage()
{
    return QStringList();
}

void CAndroidDevice::install(QString apkPath)
{
    //TODO
}

QString CAndroidDevice::getRunningService()
{
    return "";//TODO
}

QProcess *CAndroidDevice::logcat(QString patterns)
{
    return nullptr;//TODO
}

CAndroidApp::CAndroidApp(QString package, QObject *parent):
    QObject(parent),package(package)
{

}

QString CAndroidApp::getName()
{
    return "";//TODO
}

void CAndroidApp::uninstall()
{
    //TODO
}

void CAndroidApp::pmClear()
{
    //TODO
}

QString CAndroidApp::getRunningService()
{
    return "";//TODO
}

QString CAndroidApp::getInfo()
{
    return "";//TODO
}

QString CAndroidApp::getInstallPath()
{
    return ""//TODO
}
