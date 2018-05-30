#ifndef CDEVICEFORM_H
#define CDEVICEFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QPointer>
#include <QDropEvent>
#include <QMenu>
#include <QDrag>
#include "candroiddevice.h"

namespace Ui
{
class CDeviceForm;
}

class CDeviceForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceForm(CAndroidDevice * device,QWidget *parent = 0);
    ~CDeviceForm();

public slots:
    void updateDevices();

private:
    Ui::CDeviceForm *ui;
    QPointer<CAndroidDevice> devicePointer;
    QPoint mStartPoint;

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

signals:
    void menuRequested(const QString &serialNumber, const QString &path);
};

#endif // CDEVICEFORM_H
