#ifndef CDEVICEEDITFORM_H
#define CDEVICEEDITFORM_H

#include <QWidget>
#include <QMenu>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>
#include "candroiddevice.h"
#include "screenrecordoptiondialog.h"

namespace Ui
{
class CDeviceEditForm;
class CScreenPixmapItem;
}

class CScreenPixmapItem : public QObject,public QGraphicsPixmapItem
{
    Q_OBJECT

signals:
    void itemLongClick(const QPoint &pos,int duration);
    void itemClick(const QPoint &pos);
    void itemSwipe(const QPoint &startPos,const QPoint &endPos,int duration);
protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QTime pressTime;
    QPoint pressPoint;
};

class CDeviceEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceEditForm(CAndroidDevice * device,QWidget *parent = 0);
    ~CDeviceEditForm();
protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
signals:
    void processStart(const QString &title,const QString &content);
    void processEnd(int exitCode,const QString &msg);
    void screenUpdated();

private slots:
    void on_graphicsView_customContextMenuRequested(const QPoint &pos);

    void on_actionscreen_record_triggered();

    void on_actionscreen_shot_triggered();

private:
    QString deviceSerialNumber;
    Ui::CDeviceEditForm *ui;
    QPointer<CAndroidDevice> devicePointer;
    QProcess *screenSyncProcess;
    QGraphicsScene *scene;
    CScreenPixmapItem * screenItem;
    QPixmap screenPixmap;
    int scenePercent = 25;

    void inputKeyEvent(int keyCode);

    QList<QAction *> inputNumberActions;
    QList<QAction *> inputLetterActions;
};

#endif // CDEVICEEDITFORM_H
