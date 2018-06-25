#ifndef CDEVICEFILEFORM_H
#define CDEVICEFILEFORM_H

#include <QDrag>
#include <QWidget>
#include <QTableWidgetItem>
#include <QDragEnterEvent>
#include "candroiddevice.h"

namespace Ui {
class CDeviceFileForm;
class CDeviceFileTableWidget;
}

class CDeviceFileTableWidget : public QTableWidget
{
public:
    explicit CDeviceFileTableWidget(QWidget *parent = 0);
    QString getBasePath(){return basePath;}
    void setBasePath(const QString &path){this->basePath = path;}
    QString getSerialNumber(){return this->serialNumber;}
    void setSerialNumber(const QString &number){this->serialNumber = number;}
protected:
    virtual QMimeData *mimeData(const QList<QTableWidgetItem *> items) const;

private:
    QString basePath;
    QString serialNumber;
};

class CDeviceFileForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceFileForm(CAndroidDevice * device,QWidget *parent = 0);
    ~CDeviceFileForm();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

signals:
    void menuRequested(const QString &serialNumber,const QString &localPath,const QString &devicePath);

private:
    void openDir(const QString &path);

    Ui::CDeviceFileForm *ui;
    QPointer<CAndroidDevice> devicePointer;
    QString deviceSerialNumber;
    QString currentDir = "";
    QList<CAndroidFile> currentFiles;
    QStack<QString> prevPathStack;
    QStack<QString> nextPathStack;
    QPoint mStartPoint;
};

#endif // CDEVICEFILEFORM_H
