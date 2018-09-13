#include "cstartupform.h"
#include "ui_cstartupform.h"

CStartUpForm::CStartUpForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CStartUpForm)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    ui->widget->setAutoFillBackground(true);
    QPixmap pixmap(":/img/startup_bg");
    QPixmap fitpixmap=pixmap.scaled(width(),height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPalette palette = ui->widget->palette();
    palette.setBrush(QPalette::Background, QBrush(fitpixmap));
    ui->widget->setPalette(palette);

    animation = nullptr;
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
    if(animation != nullptr && animation->state() == QPropertyAnimation::Stopped) {
        animation->stop();
        animation->deleteLater();
        animation == nullptr;
    }

    animation = new QPropertyAnimation(ui->progressBar, "value");
    animation->setDuration(300);
    animation->setStartValue(ui->progressBar->value());
    animation->setEndValue(progress);
    animation->start(QAbstractAnimation::KeepWhenStopped);
}
