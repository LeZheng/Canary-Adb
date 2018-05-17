#include "cconsoleform.h"
#include "ui_cconsoleform.h"

CConsoleForm::CConsoleForm(CAndroidDevice *device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CConsoleForm)
{
    ui->setupUi(this);
    devicePointer = device;

    logProcess = nullptr;

    connect(this,&CConsoleForm::logConditionUpdated,[this]() {
        QProcess * tempProcess = this->logProcess;
        if(tempProcess != nullptr) {
            tempProcess->kill();
            this->ui->logContentTextEdit->clear();
        }
        QString logLevel = this->ui->logLevelComboBox->currentText().left(1);
        tempProcess =  this->devicePointer->logcat(
                           this->ui->logFormatComboBox->currentText(),
                           this->ui->logLevelComboBox->currentText(),
                           this->ui->filterTagLineEdit->text(),
                           this->ui->filterContentLineEdit->text(),
                           this->ui->filterPIDLineEdit->text());
        this->logProcess = tempProcess;

        connect(tempProcess,&QProcess::readyRead,[this,tempProcess]() {
            this->ui->logContentTextEdit->append(tempProcess->readAll());
        });
        connect(tempProcess,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[tempProcess]() {
            delete tempProcess;
        });
    });

    connect(this->ui->logLevelComboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->logFormatComboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->processComboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->filterPIDLineEdit,&QLineEdit::textChanged,this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->filterContentLineEdit,&QLineEdit::textChanged,this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->filterTagLineEdit,&QLineEdit::textChanged,this,&CConsoleForm::logConditionUpdated);
    emit logConditionUpdated();

    startTimer(1000);
}

CConsoleForm::~CConsoleForm()
{
    delete ui;
}
