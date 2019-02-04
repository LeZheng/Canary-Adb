#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QDesktopServices>
#include <QLabel>
#include <QUrl>
#include <QProgressDialog>
#include <QSplitter>
#include <QToolButton>
#include <QMessageBox>
#include <QToolTip>

#include "cdeviceform.h"
#include "cconsoleform.h"
#include "cfileform.h"
#include "candroiddevice.h"
#include "cdeviceeditform.h"
#include "cdevicefileform.h"
#include "cdevicefiledialog.h"
#include "cmonitorform.h"

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
signals:
    void currentFileFormChanged(CFileForm *form);
    void processStart(const QString &title,const QString &content);
    void processEnd(int exitCode,const QString &msg);
private:
    Ui::MainWindow *ui;
    QComboBox *deviceComboBox;
    QMap<QString,QStackedWidget *> deviceStackMap;
    QProgressDialog * loadingDialog;
    QPropertyAnimation *loadAnimation;
    QList<QString> serialNumberList;

    void initToolBar();
    void initFileWidget();
    void initDeviceWidget();
protected:
    virtual void closeEvent(QCloseEvent *event);
private slots:
    void changeFileViewMode(FileItemMode mode);
    void showLoadingDialog(const QString &title,const QString &content);
    void hideLoadingDialog(int exitCode,const QString &msg);
    void requestContextMenu(const QString &serialNumber,const QString &localPath,const QString &devicePath);
    void requestAppContextMenu(const QString &serialNumber, const QString &packageName);
    void installApk(const QString &serialNumber,const QString &path);
    void screenShot(const QString &serialNumber);
    void screenRecord(const QString &serialNumber);
    void pushFile(const QString &serialNumber,const QString &localPath,const QString &devicePath);
    void pullFile(const QString &serialNumber,const QString &localPath,const QString &devicePath);
};

#endif // MAINWINDOW_H
