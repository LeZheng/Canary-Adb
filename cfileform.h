#ifndef CFILEFORM_H
#define CFILEFORM_H

#include <QWidget>

namespace Ui {
class CFileForm;
}

class CFileForm : public QWidget
{
    Q_OBJECT

public:
    explicit CFileForm(QWidget *parent = 0);
    ~CFileForm();

private:
    Ui::CFileForm *ui;
};

#endif // CFILEFORM_H
