#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QDesktopServices>
#include <QUrl>

#include "cdeviceform.h"
#include "cconsoleform.h"
#include "cfileform.h"
#include "candroiddevice.h"

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

    void initToolBar();
    void initFileWidget();
    void initDeviceWidget();
    void initConsoleWidget();

private slots:
    void showFileRequestMenu(const QPoint &pos,const QModelIndex &index,QString path);
    void showDeviceRequestMenu(const QPoint &pos,QString name);
};

#endif // MAINWINDOW_H
