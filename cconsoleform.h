#ifndef CCONSOLEFORM_H
#define CCONSOLEFORM_H

#include <QWidget>

namespace Ui {
class CConsoleForm;
}

class CConsoleForm : public QWidget
{
    Q_OBJECT

public:
    explicit CConsoleForm(QWidget *parent = 0);
    ~CConsoleForm();

private:
    Ui::CConsoleForm *ui;
};

#endif // CCONSOLEFORM_H
