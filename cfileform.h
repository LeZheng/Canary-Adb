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

enum class FileItemMode{GRID,LIST,TREE};

class CFileForm : public QWidget
{
    Q_OBJECT

public:
    explicit CFileForm(QWidget *parent = 0);
    ~CFileForm();

    QFileSystemModel * getModel(){return model;}
    QAbstractItemView * getCurrentItemView();
    void setViewMode(FileItemMode mode);
    FileItemMode viewMode();
    void setSelect(bool selected);
    bool isSelect(){return isSelected;}

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
signals:
    void menuRequested(const QString &path);
    void basePathChanged(const QString &path);
    void selected();

private:
    bool isSelected = false;
    FileItemMode mode = FileItemMode::GRID;
    Ui::CFileForm *ui;
    QFileSystemModel * model;
    QAbstractItemView * fileTreeView;
    QString workPath;
    QStack<QModelIndex> historyPathStack;
};

#endif // CFILEFORM_H
