#ifndef STARTUPFORM_H
#define STARTUPFORM_H

#include <QWidget>
#include <QPropertyAnimation>

namespace Ui
{
class CStartUpForm;
}

class CStartUpForm : public QWidget
{
    Q_OBJECT

public:
    explicit CStartUpForm(QWidget *parent = 0);
    ~CStartUpForm();

public slots:
    void showMessage(const QString &message, Qt::AlignmentFlag alignment = Qt::AlignLeft, const QColor &color = Qt::black);
    void setCurrentProgress(int progress);
private:
    Ui::CStartUpForm *ui;
    QPropertyAnimation *animation;
};

#endif // STARTUPFORM_H
