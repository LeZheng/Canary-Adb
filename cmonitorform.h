#ifndef CMINOTORFORM_H
#define CMINOTORFORM_H

#include <QWidget>
#include <QtCharts>
#include "candroiddevice.h"

namespace Ui
{
class CMonitorForm;
}

class CMonitorForm : public QWidget
{
    Q_OBJECT

public:
    enum ChartType {BarChart,LineChart};
    explicit CMonitorForm(CAndroidDevice * device,QWidget *parent = 0);
    ~CMonitorForm();

signals:
    void chartTypeChanged(ChartType type);
    void legendStatusChanged(QString legendName,bool isChecked);

protected:
    virtual void timerEvent(QTimerEvent *event);

private:
    Ui::CMonitorForm *ui;
    QPointer<CAndroidDevice> devicePointer;
    QString serialNumber;
    QSet<QString> selectedMemTypeSet;
    ChartType currentType = BarChart;

    void updateMemoryChart();
    void updateProcessModel(QList<QString> processList);
    void clearSeriesAndAxis(QChart *chart);

private slots:
    void updateMemLineChart(QMap<QString,QString> memInfoMap);
    void updateMemBarChart(QMap<QString,QString> memInfoMap,QBarSeries *memBarSeries,QBarCategoryAxis *axisX,QValueAxis *axisY);
};

#endif // CMINOTORFORM_H
