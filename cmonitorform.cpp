#include "cmonitorform.h"
#include "ui_cmonitorform.h"

CMonitorForm::CMonitorForm(CAndroidDevice * device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMonitorForm)
{
    ui->setupUi(this);

    devicePointer = device;
    if(!devicePointer.isNull()) {
        serialNumber = devicePointer->serialNumber;
    }

    ui->memChartView->setChart(new QChart());
    ui->memChartView->chart()->setTitle("memory usage");
    ui->memChartView->setRenderHint(QPainter::Antialiasing);
    ui->memChartView->chart()->legend()->setVisible(true); //显示图例
    ui->memChartView->chart()->legend()->setAlignment(Qt::AlignBottom); //图例显示在下方

    connect(this,&CMonitorForm::chartTypeChanged,this,[this](ChartType type) {
        clearSeriesAndAxis(this->ui->memChartView->chart());
        currentType = type;
        updateMemoryChart();
    });

    connect(this,&CMonitorForm::legendStatusChanged,this,[this](QString legendName,bool isChecked) {
        //TODO
    });

    startTimer(500);
}

CMonitorForm::~CMonitorForm()
{
    delete ui;
}

void CMonitorForm::timerEvent(QTimerEvent *event)
{
    updateMemoryChart();
    //TODO update process model
}

void CMonitorForm::updateMemoryChart()
{
    if(!devicePointer.isNull()) {
        QChart *chart = ui->memChartView->chart();
        devicePointer->updateMemInfo();
        QMap<QString,QString> memInfoMap = devicePointer->getMemInfo();
        if(currentType == BarChart) {
            QBarSeries *memBarSeries = nullptr;
            if(!chart->series().isEmpty()) {
                memBarSeries = qobject_cast<QBarSeries *>(chart->series().at(0));
            }
            if(memBarSeries == nullptr || memBarSeries == 0) {
                memBarSeries = new QBarSeries();
                chart->addSeries(memBarSeries);
            }

            QBarCategoryAxis *axisX = nullptr;
            if(chart->axisX() != nullptr) {
                axisX = qobject_cast<QBarCategoryAxis *>(chart->axisX());
            }
            if(axisX == nullptr || axisX == 0) {
                axisX = new QBarCategoryAxis;
                ui->memChartView->chart()->setAxisX(axisX,memBarSeries);
            }

            QValueAxis *axisY = nullptr;// new QValueAxis();
            if(chart->axisY() != nullptr) {
                axisY = qobject_cast<QValueAxis *>(chart->axisY());
            }
            if(axisY == nullptr || axisY == 0) {
                axisY = new QValueAxis();
                ui->memChartView->chart()->setAxisY(axisY,memBarSeries);
            }
            axisY->setLabelFormat("%d");
            updateMemBarChart(memInfoMap,memBarSeries,axisX,axisY);
        } else if(currentType == LineChart) {
            updateMemLineChart(memInfoMap);
        }

    }
}

void CMonitorForm::updateProcessModel(QList<QString> processList)
{
    //TODO
}

void CMonitorForm::clearSeriesAndAxis(QChart *chart)
{
    if(chart != nullptr) {
        chart->removeAllSeries(); //删除所有序列
        chart->removeAxis(chart->axisX()); //删除坐标轴
        chart->removeAxis(chart->axisY()); //删除坐标轴
    }
}

void CMonitorForm::updateMemLineChart(QMap<QString, QString> memInfoMap)
{
    //TODO
}

void CMonitorForm::updateMemBarChart(QMap<QString,QString> memInfoMap,QBarSeries *memBarSeries,QBarCategoryAxis *axisX,QValueAxis *axisY)
{
    bool xAxisIsEmpty = axisX->categories().isEmpty();
    if(!memInfoMap.isEmpty()) {
        QMapIterator<QString,QString> memInfoIter(memInfoMap);

        memBarSeries->clear();
        int maxMemValue = 1024 * 1024;
        while (memInfoIter.hasNext()) {
            memInfoIter.next();
            QString memStr = memInfoIter.value();
            if(memStr.endsWith(" kB")) {
                memStr = memStr.left(memStr.size() - 3);
            }
            int memSize = memStr.toInt();
            while(maxMemValue < memSize) {
                maxMemValue *= 2;
            }
            QBarSet *memInfoSet = new QBarSet(memInfoIter.key());
            memInfoSet->append(memSize);
            memBarSeries->append(memInfoSet);

            if(xAxisIsEmpty)
                axisX->append(memInfoIter.key());
        }
        axisY->setRange(0,maxMemValue);
    }
}
