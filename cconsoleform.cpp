#include "cconsoleform.h"
#include "ui_cconsoleform.h"

CConsoleForm::CConsoleForm(CAndroidDevice *device,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CConsoleForm)
{
    ui->setupUi(this);
    devicePointer = device;
    if(!devicePointer.isNull()) {
        deviceSerialNumber = devicePointer->serialNumber;
        this->isDeviceConnected = true;
    }

    logProcess = nullptr;

    connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListUpdated,this,[this]() {
        QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
        QListIterator<CAndroidDevice *> deviceIter(deviceList);
        while(deviceIter.hasNext()) {
            CAndroidDevice * device = deviceIter.next();
            if(device->serialNumber == this->deviceSerialNumber) {
                if(!this->isDeviceConnected) {
                    this->devicePointer = device;
                    this->isDeviceConnected = true;
                    this->ui->filterContentLineEdit->setEnabled(true);
                    this->ui->filterTagLineEdit->setEnabled(true);
                    this->ui->filterPIDLineEdit->setEnabled(true);
                    this->ui->logLevelComboBox->setEnabled(true);
                    this->ui->logFormatComboBox->setEnabled(true);
                }
                if(this->logProcess == nullptr) {
                    emit logConditionUpdated();
                }
                return;
            }
        }
        this->isDeviceConnected = false;
        this->ui->filterContentLineEdit->setDisabled(true);
        this->ui->filterTagLineEdit->setDisabled(true);
        this->ui->filterPIDLineEdit->setDisabled(true);
        this->ui->logLevelComboBox->setDisabled(true);
        this->ui->logFormatComboBox->setDisabled(true);
    });

    connect(this,&CConsoleForm::logConditionUpdated,[this]() {
        QProcess * tempProcess = this->logProcess;
        if(tempProcess != nullptr) {
            tempProcess->kill();
        }
        this->ui->logContentTextEdit->clear();
        tempProcess = this->devicePointer->logcat(
                          this->ui->logFormatComboBox->currentText(),
                          this->ui->logLevelComboBox->currentText(),
                          this->ui->filterTagLineEdit->text(),
                          this->ui->filterContentLineEdit->text(),
                          this->ui->filterPIDLineEdit->text());
        this->logProcess = tempProcess;

        connect(tempProcess,&QProcess::readyRead,this,[this,tempProcess]() {
            this->ui->logContentTextEdit->append(tempProcess->readAll());
        });
        connect(tempProcess,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),tempProcess,[tempProcess,this]() {
            if(tempProcess == this->logProcess) {
                this->logProcess = nullptr;
            }
            delete tempProcess;
        });
    });

    connect(this->ui->logLevelComboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->logFormatComboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->filterPIDLineEdit,&QLineEdit::textChanged,this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->filterContentLineEdit,&QLineEdit::textChanged,this,&CConsoleForm::logConditionUpdated);
    connect(this->ui->filterTagLineEdit,&QLineEdit::textChanged,this,&CConsoleForm::logConditionUpdated);
    emit logConditionUpdated();

    connect(this->ui->logContentTextEdit,&QTextEdit::customContextMenuRequested,[this]() {
        QMenu menu;
        if(!this->ui->logContentTextEdit->textCursor().selectedText().isEmpty()) {
            menu.addAction(tr("copy"),[this]() {
                QClipboard *clipboard = QGuiApplication::clipboard();
                clipboard->setText(this->ui->logContentTextEdit->textCursor().selectedText());
            });
        }
        menu.addAction(tr("select all"),[this]() {
            this->ui->logContentTextEdit->selectAll();
        });
        menu.addAction(tr("clear"),[this]() {
            this->ui->logContentTextEdit->clear();
        });
        menu.addAction(tr("export to file"),[this]() {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               QDir::homePath(),
                               tr("Text files (*.txt)"));
            QFile saveFile(fileName);
            if(saveFile.open(QFile::WriteOnly | QFile::Append)) {
                saveFile.write(this->ui->logContentTextEdit->toPlainText().toUtf8());
            } else {
                //TODO error
            }
        });
        menu.exec(QCursor::pos());
    });
}

CConsoleForm::~CConsoleForm()
{
    this->disconnect();
    if(logProcess != nullptr){
        logProcess->kill();
    }
    delete ui;
}
