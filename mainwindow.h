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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CFileForm *fileForm;
    QMap<QString,QTabWidget *> deviceTabMap;
    QProgressDialog * loadingDialog;
    QPropertyAnimation *loadAnimation;

    void initToolBar();
    void initFileWidget();
    void initDeviceWidget();
protected:
    virtual void closeEvent(QCloseEvent *event);
private slots:
    void openDeviceDetailView(CAndroidDevice *device);
    void showLoadingDialog(const QString &title,const QString &content);
    void hideLoadingDialog(int exitCode,const QString &msg);
};

#endif // MAINWINDOW_H
