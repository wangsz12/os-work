#include "newprocesswindow.h"
#include "ui_newprocesswindow.h"
#include <ctime>
#include "qmessagebox.h"
#include "Process_.h"
#include "enums.h"
#include <iostream>
using namespace std;

NewProcessWindow::NewProcessWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProcessWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}

NewProcessWindow::~NewProcessWindow()
{
    delete ui;
}

void NewProcessWindow::set_os_ptr(OS *os) {
    this->os = os;
}

void NewProcessWindow::init() {
    QRegExp regx("[0-9]+$");
    QValidator* validator;

    ui->nameEdit->setText("");
    validator = new QRegExpValidator(regx, ui->priorityEdit);
    ui->priorityEdit->setValidator(validator);
    ui->priorityEdit->setText("");

    validator = new QRegExpValidator(regx, ui->memoryEdit);
    ui->memoryEdit->setValidator(validator);
    ui->memoryEdit->setText("");

    validator = new QRegExpValidator(regx, ui->timeEdit);
    ui->timeEdit->setValidator(validator);
    ui->timeEdit->setText("");

    ui->preProcessSelect->setEnabled(false);
    ui->sucProcessSelect->setEnabled(false);

    ui->preProcessSelect->clear();
    ui->sucProcessSelect->clear();
    ui->preProcessSelect->addItem("无");
    ui->sucProcessSelect->addItem("无");

    ui->propsSelect->setCurrentIndex(0);
    ui->preProcessSelect->setCurrentIndex(0);
    ui->sucProcessSelect->setCurrentIndex(0);
}

void NewProcessWindow::before_show() {
    init();

    for (list<Process*>::iterator i = os->all.begin(); i != os->all.end(); ++i) {
        if ((*i)->pcb->state != TERMINATED) {
            ui->preProcessSelect->addItem((*i)->pcb->name);
            ui->sucProcessSelect->addItem((*i)->pcb->name);
        }
    }

//    ui->sucProcessSelect->addItem("---");
}

void NewProcessWindow::on_randomBtn_clicked()
{
    ui->nameEdit->setText("process" + QString::number(qrand() % 100));
    ui->priorityEdit->setText(QString::number(qrand() % 20 + 1));
    ui->memoryEdit->setText(QString::number(qrand() % 400 + 1));
    ui->timeEdit->setText(QString::number(qrand() % 10 + 1));
}

bool NewProcessWindow::verify() {
    if (ui->nameEdit->text() == "") {
        QMessageBox::critical(this, "添加进程", "请输入进程名");
        return false;
    }

    for (list<Process*>::iterator i = os->all.begin(); i != os->all.end(); ++i) {
        if ((*i)->pcb->name == ui->nameEdit->text()) {
            QMessageBox::critical(this, "添加进程", "进程名不允许重名");
            return false;
        }
    }

    if (ui->priorityEdit->text() == "") {
        QMessageBox::critical(this, "添加进程", "请输入优先级");
        return false;
    }

    if (ui->memoryEdit->text() == "") {
        QMessageBox::critical(this, "添加进程", "请输入所占内存");
        return false;
    }

    if (ui->timeEdit->text() == "") {
        QMessageBox::critical(this, "添加进程", "请输入所需时间");
        return false;
    }

    if (ui->propsSelect->currentIndex() == 1) {
        if (ui->preProcessSelect->currentText() == "无" && ui->sucProcessSelect->currentText() == "无") {
            QMessageBox::critical(this, "添加进程", "同步进程需要至少设置前驱进程和后继进程中的一个");
            return false;
        }

        if (ui->preProcessSelect->currentIndex() == ui->sucProcessSelect->currentIndex()){
            QMessageBox::critical(this, "添加进程", "前驱进程不能和后继进程是同一个");
            return false;
        }

        if (ui->preProcessSelect->currentText() != "无") {
            for (list<Process*>::iterator i = os->all.begin(); i != os->all.end(); ++i) {
                if ((*i)->pcb->name == ui->preProcessSelect->currentText() && (*i)->pcb->suc_process) {
                    QMessageBox::critical(this, "添加进程", "进程 " + ui->preProcessSelect->currentText() + " 已有后继进程");
                    return false;
                }
            }
        }

        if (ui->sucProcessSelect->currentText() != "无") {
            for (list<Process*>::iterator i = os->all.begin(); i != os->all.end(); ++i) {
                if ((*i)->pcb->name == ui->sucProcessSelect->currentText() && (*i)->pcb->pre_process) {
                    QMessageBox::critical(this, "添加进程", "进程 " + ui->sucProcessSelect->currentText() + " 已有前驱进程");
                    return false;
                }
            }
        }
    }

    return true;
}

void NewProcessWindow::on_confirmBtn_clicked()
{
    if (!verify()) return;

    Process* pre = nullptr;
    Process* suc = nullptr;
    const QString pre_name = ui->preProcessSelect->currentText();
    const QString suc_name = ui->sucProcessSelect->currentText();

    if (ui->propsSelect->currentIndex() == 1 && (pre_name != "无" || suc_name != "无")) {
        for (list<Process*>::iterator i = os->all.begin(); i != os->all.end(); ++i) {
            if ((*i)->pcb->name == pre_name) pre = *i;
            if ((*i)->pcb->name == suc_name) suc = *i;
        }
    }

    emit addProcessSignal(
                ui->nameEdit->text(),
                ui->priorityEdit->text().toInt(),
                ui->memoryEdit->text().toInt(),
                ui->timeEdit->text().toInt(),
                ui->propsSelect->currentIndex(),
                pre,
                suc);
    close();
}

void NewProcessWindow::on_propsSelect_activated(int index)
{
    ui->preProcessSelect->setEnabled(index);
    ui->sucProcessSelect->setEnabled(index);

    if (index == 0) {
        ui->preProcessSelect->setCurrentIndex(0);
        ui->sucProcessSelect->setCurrentIndex(0);
    }
}

void NewProcessWindow::on_clearBtn_clicked()
{
    init();
}
