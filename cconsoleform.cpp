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
    }
    logProcess = nullptr;

    connect(device,&CAndroidDevice::destroyed,this,&CConsoleForm::deviceDisconnected);
    connect(CAndroidContext::getInstance(),&CAndroidContext::deviceListUpdated,this,[this]() {
        QList<CAndroidDevice *> deviceList = CAndroidContext::getDevices();
        QListIterator<CAndroidDevice *> deviceIter(deviceList);
        while(deviceIter.hasNext()) {
            CAndroidDevice * device = deviceIter.next();
            if(device->serialNumber == this->deviceSerialNumber) {
                if(this->devicePointer.isNull()) {
                    this->devicePointer = device;
                    emit deviceConnected();
                }
                return;
            }
        }
        this->devicePointer.clear();
        emit deviceDisconnected();
    });

    connect(ui->clearToolButton,&QToolButton::clicked,ui->logContentTextEdit,&QTextEdit::clear);
    connect(this->ui->logContentTextEdit,&QTextEdit::customContextMenuRequested,[this]() {
        QMenu menu;
        if(!this->ui->logContentTextEdit->textCursor().selectedText().isEmpty()) {
            menu.insertAction(nullptr,ui->actioncopy);
        }
        menu.insertAction(nullptr,ui->actionselect_all);
        menu.insertAction(nullptr,ui->actionclear);
        menu.insertAction(nullptr,ui->actionsave_to_file);
        menu.exec(QCursor::pos());
    });

    machine = new QStateMachine(this);
    connectState = new QState();
    disconnectState = new QState();
    recordingState = new QState();
    pauseState = new QState();

    connectState->addTransition(this,&CConsoleForm::deviceDisconnected,disconnectState);
    recordingState->addTransition(this,&CConsoleForm::deviceDisconnected,disconnectState);
    pauseState->addTransition(this,&CConsoleForm::deviceDisconnected,disconnectState);
    disconnectState->addTransition(this,&CConsoleForm::deviceConnected,connectState);
    connectState->addTransition(ui->recordToolButton,&QToolButton::clicked,recordingState);
    recordingState->addTransition(ui->recordToolButton,&QToolButton::clicked,pauseState);
    pauseState->addTransition(ui->recordToolButton,&QToolButton::clicked,recordingState);

    connectState->assignProperty(ui->filterContentLineEdit,"toolTip",tr("the content used to filter"));
    connectState->assignProperty(ui->filterPIDLineEdit,"toolTip",tr("the process id used to filter"));
    connectState->assignProperty(ui->filterTagLineEdit,"toolTip",tr("the tag used to filter"));
    connectState->assignProperty(ui->logLevelComboBox,"toolTip",tr("the level used to control log content"));
    connectState->assignProperty(ui->logFormatComboBox,"toolTip",tr("the format used to control log content"));
    connectState->assignProperty(ui->recordToolButton,"toolTip",tr("click to start record log immediately"));
    connectState->assignProperty(ui->filterContentLineEdit,"readOnly",false);
    connectState->assignProperty(ui->filterPIDLineEdit,"readOnly",false);
    connectState->assignProperty(ui->filterTagLineEdit,"readOnly",false);
    connectState->assignProperty(ui->logLevelComboBox,"enabled",true);
    connectState->assignProperty(ui->logFormatComboBox,"enabled",true);
    connectState->assignProperty(ui->writeFileCheckBox,"enabled",true);
    connectState->assignProperty(ui->recordToolButton,"enabled",true);
    connectState->assignProperty(ui->recordToolButton,"text",tr("start"));
    connectState->assignProperty(ui->recordToolButton,"icon",QIcon(":/img/ic_play"));

    recordingState->assignProperty(ui->filterContentLineEdit,"toolTip",tr("not available at run time"));
    recordingState->assignProperty(ui->filterPIDLineEdit,"toolTip",tr("not available at run time"));
    recordingState->assignProperty(ui->filterTagLineEdit,"toolTip",tr("not available at run time"));
    recordingState->assignProperty(ui->logLevelComboBox,"toolTip",tr("not available at run time"));
    recordingState->assignProperty(ui->logFormatComboBox,"toolTip",tr("not available at run time"));
    recordingState->assignProperty(ui->recordToolButton,"toolTip",tr("click to stop record log immediately"));
    recordingState->assignProperty(ui->filterContentLineEdit,"readOnly",true);
    recordingState->assignProperty(ui->filterPIDLineEdit,"readOnly",true);
    recordingState->assignProperty(ui->filterTagLineEdit,"readOnly",true);
    recordingState->assignProperty(ui->logLevelComboBox,"enabled",false);
    recordingState->assignProperty(ui->logFormatComboBox,"enabled",false);
    recordingState->assignProperty(ui->writeFileCheckBox,"enabled",false);
    recordingState->assignProperty(ui->recordToolButton,"enable",true);
    recordingState->assignProperty(ui->recordToolButton,"text",tr("stop"));
    recordingState->assignProperty(ui->recordToolButton,"icon",QIcon(":/img/ic_record"));
    connect(recordingState,&QState::entered,this,[this]() {
        QProcess * tempProcess = this->logProcess;
        if(tempProcess != nullptr) {
            tempProcess->kill();
        }
        tempProcess = this->devicePointer->logcat(
                          this->ui->logFormatComboBox->currentText(),
                          this->ui->logLevelComboBox->currentText(),
                          this->ui->filterTagLineEdit->text(),
                          this->ui->filterContentLineEdit->text(),
                          this->ui->filterPIDLineEdit->text());
        this->logProcess = tempProcess;

        QDir(".").mkdir("adb_log");
        QString fileName = QString("log_%1_%2.txt")
                           .arg(this->devicePointer->getModel())
                           .arg(QDateTime::currentDateTime().toString("yyyy_MM_dd__hh_mm_ss"));
        auto f = new QFile (QString("%1/%2").arg("./adb_log").arg(fileName));
        if(ui->writeFileCheckBox->isChecked()) {
            f->open(QIODevice::WriteOnly);
        }

        connect(tempProcess,&QProcess::readyRead,this,[this,tempProcess,f]() {
            auto dataBytes = tempProcess->readAll();
            if(ui->showConsoleCheckBox->isChecked()) {
                QString logText = QString(dataBytes).replace('\r', "");
                this->ui->logContentTextEdit->append(logText);
            }
            if(f->isOpen()) {
                f->write(dataBytes);
            }
        });
        connect(tempProcess,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),tempProcess,[tempProcess,this]() {
            if(tempProcess == this->logProcess) {
                this->logProcess = nullptr;
            }
            delete tempProcess;
        });
    });
    connect(recordingState,&QState::exited,this,[this]() {
        if(this->logProcess != nullptr) {
            this->logProcess->kill();
            this->logProcess = nullptr;
        }
    });

    pauseState->assignProperty(ui->filterContentLineEdit,"toolTip",tr("the content used to filter"));
    pauseState->assignProperty(ui->filterPIDLineEdit,"toolTip",tr("the process id used to filter"));
    pauseState->assignProperty(ui->filterTagLineEdit,"toolTip",tr("the tag used to filter"));
    pauseState->assignProperty(ui->logLevelComboBox,"toolTip",tr("the level used to control log content"));
    pauseState->assignProperty(ui->logFormatComboBox,"toolTip",tr("the format used to control log content"));
    pauseState->assignProperty(ui->recordToolButton,"toolTip",tr("click to start record log immediately"));
    pauseState->assignProperty(ui->filterContentLineEdit,"readOnly",false);
    pauseState->assignProperty(ui->filterPIDLineEdit,"readOnly",false);
    pauseState->assignProperty(ui->filterTagLineEdit,"readOnly",false);
    pauseState->assignProperty(ui->logLevelComboBox,"enabled",true);
    pauseState->assignProperty(ui->logFormatComboBox,"enabled",true);
    pauseState->assignProperty(ui->recordToolButton,"enabled",true);
    pauseState->assignProperty(ui->writeFileCheckBox,"enabled",true);
    pauseState->assignProperty(ui->recordToolButton,"text",tr("start"));
    pauseState->assignProperty(ui->recordToolButton,"icon",QIcon(":/img/ic_play"));

    disconnectState->assignProperty(ui->filterContentLineEdit,"toolTip",tr("device not connected"));
    disconnectState->assignProperty(ui->filterPIDLineEdit,"toolTip",tr("device not connected"));
    disconnectState->assignProperty(ui->filterTagLineEdit,"toolTip",tr("device not connected"));
    disconnectState->assignProperty(ui->logLevelComboBox,"toolTip",tr("device not connected"));
    disconnectState->assignProperty(ui->logFormatComboBox,"toolTip",tr("device not connected"));
    disconnectState->assignProperty(ui->recordToolButton,"toolTip",tr("device not connected"));
    disconnectState->assignProperty(ui->filterContentLineEdit,"readOnly",true);
    disconnectState->assignProperty(ui->filterPIDLineEdit,"readOnly",true);
    disconnectState->assignProperty(ui->filterTagLineEdit,"readOnly",true);
    disconnectState->assignProperty(ui->logLevelComboBox,"enabled",false);
    disconnectState->assignProperty(ui->logFormatComboBox,"enabled",false);
    disconnectState->assignProperty(ui->writeFileCheckBox,"enabled",false);
    disconnectState->assignProperty(ui->recordToolButton,"enabled",false);

    machine->addState(connectState);
    machine->addState(disconnectState);
    machine->addState(recordingState);
    machine->addState(pauseState);

    machine->setInitialState(connectState);
    machine->start();
}

CConsoleForm::~CConsoleForm()
{
    this->disconnect();
    if(logProcess != nullptr) {
        logProcess->kill();
    }
    delete ui;
}

void CConsoleForm::on_actioncopy_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(this->ui->logContentTextEdit->textCursor().selectedText());
}

void CConsoleForm::on_actionsave_to_file_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                       QDir::homePath(),
                       tr("Text files (*.txt)"));
    QFile saveFile(fileName);
    if(saveFile.open(QFile::WriteOnly | QFile::Append)) {
        saveFile.write(this->ui->logContentTextEdit->toPlainText().toUtf8());
    } else {
        //TODO error
    }
}
