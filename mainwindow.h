#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QDesktopServices>
#include <QLabel>
#include <QUrl>
#include <QProgressDialog>

#include "cdeviceform.h"
#include "cconsoleform.h"
#include "cfileform.h"
#include "candroiddevice.h"
#include "cdeviceeditform.h"

namespace Ui
{
class MainWindow;
}

enum class DetailViewType{LOG,SCREEN};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void processStart(const QString &title,const QString &content);
    void processEnd(int exitCode,const QString &msg);
private:
    Ui::MainWindow *ui;
    QList<CFileForm *> fileFormList;
    QMap<QString,QTabWidget *> deviceTabMap;
    QProgressDialog * loadingDialog;
    QPropertyAnimation *loadAnimation;

    void initToolBar();
    void initFileWidget();
    void initDeviceWidget();
protected:
    virtual void closeEvent(QCloseEvent *event);
private slots:
    void changeFileViewMode(FileItemMode mode);
    void openDeviceDetailView(CAndroidDevice *device,DetailViewType type);
    void showLoadingDialog(const QString &title,const QString &content);
    void hideLoadingDialog(int exitCode,const QString &msg);
    void requestContextMenu(const QString &serialNumber,const QString &path);
    void installApk(const QString &serialNumber,const QString &path);
    void screenShot(const QString &serialNumber);
    void screenRecord(const QString &serialNumber);
};

#endif // MAINWINDOW_H
