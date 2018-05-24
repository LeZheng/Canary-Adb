#ifndef CDEVICEEDITFORM_H
#define CDEVICEEDITFORM_H

#include <QWidget>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include "candroiddevice.h"
#include "screenrecordoptiondialog.h"

namespace Ui {
class CDeviceEditForm;
}

class CDeviceEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceEditForm(CAndroidDevice * device,QWidget *parent = 0);
    ~CDeviceEditForm();

signals:
    void processStart(const QString &title,const QString &content);
    void processEnd(int exitCode,const QString &msg);
    void screenUpdated();

private:
    QString deviceSerialNumber;
    Ui::CDeviceEditForm *ui;
    QPointer<CAndroidDevice> devicePointer;
    QProcess *screenSyncProcess;
    QGraphicsScene *scene;
    QGraphicsPixmapItem * screenItem;
    QPixmap screenPixmap;
    int scenePercent = 25;

    void inputKeyEvent(int keyCode);
};

#endif // CDEVICEEDITFORM_H
