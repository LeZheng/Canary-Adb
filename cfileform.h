#ifndef CFILEFORM_H
#define CFILEFORM_H

#include <QWidget>
#include <QFileSystemModel>
#include <QStack>
#include <QTreeView>
#include <QUrl>
#include <QDesktopServices>
#include <QMenu>
#include <QFileDialog>
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
    QAbstractItemView * getTreeView(){return fileTreeView;}

signals:
    void menuRequested(const QString &path);
    void basePathChanged(const QString &path);

private:
    Ui::CFileForm *ui;
    QFileSystemModel * model;
    QAbstractItemView * fileTreeView;
    QString workPath;
    QStack<QModelIndex> historyPathStack;
};

#endif // CFILEFORM_H
