#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>

#include "cdeviceform.h"
#include "cconsoleform.h"
#include "cfileform.h"

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

    void initToolBar();
    void initFileWidget();
    void initDeviceWidget();
    void initConsoleWidget();
};

#endif // MAINWINDOW_H
