#-------------------------------------------------
#
# Project created by QtCreator 2018-04-08T19:15:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Canary-Adb
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS Qt_NO_CAST_FROM_ASCII

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    cfileform.cpp \
    cconsoleform.cpp \
    cdeviceform.cpp \
    candroiddevice.cpp \
    cdeviceeditform.cpp \
    screenrecordoptiondialog.cpp \
    cdevicefileform.cpp \
    cdevicefiledialog.cpp \
    cstartupform.cpp

HEADERS += \
        mainwindow.h \
    cfileform.h \
    cconsoleform.h \
    cdeviceform.h \
    candroiddevice.h \
    cdeviceeditform.h \
    screenrecordoptiondialog.h \
    cdevicefileform.h \
    cdevicefiledialog.h \
    cstartupform.h

FORMS += \
        mainwindow.ui \
    cfileform.ui \
    cconsoleform.ui \
    cdeviceform.ui \
    cdeviceeditform.ui \
    screenrecordoptiondialog.ui \
    cdevicefileform.ui \
    cdevicefiledialog.ui \
    cstartupform.ui

RESOURCES += \
    canary-img.qrc

TRANSLATIONS = canary_zh_CN.ts
