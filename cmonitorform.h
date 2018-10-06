#ifndef CMINOTORFORM_H
#define CMINOTORFORM_H

#include <QWidget>
#include <QtCharts>

namespace Ui
{
class CMonitorForm;
}

class CMonitorForm : public QWidget
{
    Q_OBJECT

public:
    explicit CMonitorForm(QWidget *parent = 0);
    ~CMonitorForm();

private:
    Ui::CMonitorForm *ui;
};

#endif // CMINOTORFORM_H
