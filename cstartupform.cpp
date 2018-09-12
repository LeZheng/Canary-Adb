#include "cstartupform.h"
#include "ui_cstartupform.h"

CStartUpForm::CStartUpForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CStartUpForm)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setAutoFillBackground(true);
    QPixmap pixmap(":/img/startup_bg");
    QPixmap fitpixmap=pixmap.scaled(width(),height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPalette palette = ui->widget->palette();
    palette.setBrush(QPalette::Background, QBrush(fitpixmap));
    ui->widget->setPalette(palette);
}

CStartUpForm::~CStartUpForm()
{
    delete ui;
}

void CStartUpForm::showMessage(const QString &message, Qt::AlignmentFlag alignment, const QColor &color)
{
    ui->label->setText(message);
    ui->label->setAlignment(alignment);
    QPalette palette = ui->label->palette();
    palette.setColor(QPalette::Text,color);
}

void CStartUpForm::setCurrentProgress(int progress)
{
    ui->progressBar->setValue(progress);
}
