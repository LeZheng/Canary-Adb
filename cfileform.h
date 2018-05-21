#ifndef CFILEFORM_H
#define CFILEFORM_H

#include <QWidget>
#include <QFileSystemModel>
#include <QStack>
#include <QTreeView>
#include <QUrl>
#include <QDesktopServices>
#include <QMenu>
#include "candroiddevice.h"

namespace Ui
{
class CFileForm;
}

class CFileForm : public QWidget
{
    Q_OBJECT

public:
    explicit CFileForm(QWidget *parent = 0);
    ~CFileForm();

    QFileSystemModel * getModel(){return model;}
    QTreeView * getTreeView(){return fileTreeView;}

signals:
    void processStart(const QString &title,const QString &content);
    void processEnd(int exitCode,const QString &msg);

private:
    Ui::CFileForm *ui;
    QFileSystemModel * model;
    QTreeView * fileTreeView;
    QString workPath;
    QStack<QModelIndex> historyPathStack;

    void showFileRequestMenu(const QPoint &pos);
};

#endif // CFILEFORM_H
