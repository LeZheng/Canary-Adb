#ifndef CFILEFORM_H
#define CFILEFORM_H

#include <QWidget>
#include <QDirModel>
#include <QStack>

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

signals:
    void itemDoubleClicked(QString path);
    void itemMenuRequested(const QPoint &pos,QString path);

private:
    Ui::CFileForm *ui;
    QDirModel * model;
    QString workPath;
    QStack<QModelIndex> historyPathStack;
};

#endif // CFILEFORM_H
