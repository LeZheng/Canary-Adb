#ifndef CFILEFORM_H
#define CFILEFORM_H

#include <QWidget>
#include <QFileSystemModel>
#include <QStack>
#include <QTreeView>

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
    void itemDoubleClicked(QString path);
    void itemMenuRequested(const QPoint &pos,const QModelIndex &index,QString path);

private:
    Ui::CFileForm *ui;
    QFileSystemModel * model;
    QTreeView * fileTreeView;
    QString workPath;
    QStack<QModelIndex> historyPathStack;
};

#endif // CFILEFORM_H
