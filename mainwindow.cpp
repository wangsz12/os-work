#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qpainter.h"
#include "Process_.h"
#include "Logger.h"
#include <iostream>
#include "enums.h"
#include "qmessagebox.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    const int colors[][3] = {
        {25,202,173},
        {255,192,203},
        {238,130,238},
        {255,140,0},
        {135,206,250},
        {255,255,0},
        {205,220,57},
        {255,193,7},
        {0,150,136},
        {255,87,34}
    };

    for (int i = 0; i< 10; ++i) {
        memoryDrawer.colors[i] = QColor(colors[i][0], colors[i][1], colors[i][2]);
    }

    ui->setupUi(this);

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());

    QTableWidget* lists[5] = {ui->readyList, ui->waitingList, ui->backupList, ui->suspendedList, ui->memoryTable};
    for (int i = 0; i < 5; ++i) {
        lists[i]->verticalHeader()->setHidden(true);
        lists[i]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        lists[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    os = new OS(1000);

    newProcessWindow = new NewProcessWindow;
    newProcessWindow->set_os_ptr(os);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::gray, 1));
    painter.setBrush(QBrush(Qt::white));
    painter.translate(30, 10);
    painter.drawRect(0, 355, 805, 30);
//    painter.drawRect(727, 510, 30, 30);
    painter.save();

    painter.setFont(QFont("微软雅黑", 10));
    painter.setPen(Qt::black);
    for (int m = 0; m <= os->memory->size; m += 100) {
        double proportion = double(m) / os->memory->size;
        QString str = "| ";
        str += QString::number(m);
        painter.drawText(proportion * memoryDrawer.totalPixel, 400, str);
    }
    painter.restore();

    if (memoryDrawer.first) {
        memoryDrawer.first = false;
        return;
    }

    list<MemoryDisplayBlock*>::iterator i = memoryDrawer.blocks.begin();
    while (i != memoryDrawer.blocks.end()) {
        if ((*i)->status == ALLOCATE) {
            painter.setPen(QPen((*i)->color, 1));
            painter.setBrush(QBrush((*i)->color));
            painter.save();
            painter.drawRect((*i)->begin + 1, 357, (*i)->width, 26);

            painter.setFont(QFont("微软雅黑", 10));
            painter.setPen(Qt::black);
            painter.drawText((*i)->begin + 5, 375, (*i)->name);
            painter.restore();

            ++i;
        }
        else {
            painter.setPen(QPen(Qt::white, 1));
            painter.setBrush(QBrush(Qt::white));
            painter.drawRect((*i)->begin + 1, 357, (*i)->width, 26);

            i = memoryDrawer.blocks.erase(i);
        }
    }
}

void MainWindow::initWindow() {
    ui->usedLabel->setText("0");
    ui->totalLabel->setText(QString::number(os->memory->size));

    onClearRunningProcess(1);
    onClearRunningProcess(2);

    os->init();
}

void MainWindow::onTimeout(){
    os->add_process_sync();
    os->load_process_from_backup();
    os->wake_process_from_waiting();
    os->execute();
}

void MainWindow::onLog(QString info) {
    ui->systemLog->addItem(info);
    ui->systemLog->scrollToBottom();
}

void MainWindow::onSetRunningProcess(int which, Process* p) {
    double progress = double(p->running_time) / p->pcb->demand_time;
    if (which == 1) {
        ui->pidLabel->setText(QString::number(p->pcb->pid));
        ui->processNameLabel->setText(p->pcb->name);
        ui->priorityLabel->setText(QString::number(p->pcb->priority));
        ui->demandTimeLabel->setText(QString::number(p->pcb->demand_time));
        ui->runningTimeLabel->setText(QString::number(p->running_time));
        ui->processPropertyLabel->setText(p->pcb->process_property == INDEPENDENT ? "独立进程" : "同步进程");
        ui->progressBar->setValue(progress * 100);
    }
    else {
        ui->pidLabel_2->setText(QString::number(p->pcb->pid));
        ui->processNameLabel_2->setText(p->pcb->name);
        ui->priorityLabel_2->setText(QString::number(p->pcb->priority));
        ui->demandTimeLabel_2->setText(QString::number(p->pcb->demand_time));
        ui->runningTimeLabel_2->setText(QString::number(p->running_time));
        ui->processPropertyLabel_2->setText(p->pcb->process_property == INDEPENDENT ? "独立进程" : "同步进程");
        ui->progressBar_2->setValue(progress * 100);
    }
}

void MainWindow::onSetMemory(Process* p, MemoryAllocateFlag flag) {
    ui->usedLabel->setText(QString::number(os->memory->used));

    int begin = memoryDrawer.totalPixel * double(p->memory_begin) / os->memory->size;
    int width = memoryDrawer.totalPixel * double(p->size) / os->memory->size;
    memoryDrawer.which = (memoryDrawer.which + 1) % 6;
    QColor color(memoryDrawer.colors[memoryDrawer.which]);

    if (flag == ALLOCATE) {
        memoryDrawer.blocks.push_back(new MemoryDisplayBlock(p, begin, width, color, flag));
    }
    else {
        list<MemoryDisplayBlock*>::iterator i = memoryDrawer.blocks.begin();
        while (i != memoryDrawer.blocks.end()) {
            if ((*i)->pid == p->pcb->pid) {
                i = memoryDrawer.blocks.erase(i);
            }
            else {
                ++i;
            }
        }
    }
    update();
}

void MainWindow::onAddProcess(QString name, int priority, int memory, int time, int props, Process* pre, Process* suc) {
    if (props == 0)
        os->add_process(new Process(name, time, priority, memory));
    else
        os->add_process(new Process(name, time, priority, memory, pre, suc));
}

void MainWindow::onClearRunningProcess(int which) {
    if (which == 1) {
        ui->pidLabel->setText("---");
        ui->processNameLabel->setText("---");
        ui->priorityLabel->setText("---");
        ui->demandTimeLabel->setText("---");
        ui->runningTimeLabel->setText("---");
        ui->memoryLabel->setText("---");
        ui->processPropertyLabel->setText("---");
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(0);
    }
    else {
        ui->pidLabel_2->setText("---");
        ui->processNameLabel_2->setText("---");
        ui->priorityLabel_2->setText("---");
        ui->demandTimeLabel_2->setText("---");
        ui->runningTimeLabel_2->setText("---");
        ui->memoryLabel_2->setText("---");
        ui->processPropertyLabel_2->setText("---");
        ui->progressBar_2->setMinimum(0);
        ui->progressBar_2->setMaximum(100);
        ui->progressBar_2->setValue(0);
    }
}

void MainWindow::onUpdateReadyList() {
    const int rows = ui->readyList->rowCount();
    for (int i = 0; i< rows; ++i) {
        ui->readyList->removeRow(0);
    }

    QTableWidgetItem* item;
    Process* p;
    priority_queue<Process*, vector<Process*>, PriorityCompare> temp(os->ready);
    while(!temp.empty()) {
        p = temp.top();
        temp.pop();
        int row = ui->readyList->rowCount();
        ui->readyList->insertRow(row);
        ui->readyList->setItem(row, 0, new QTableWidgetItem(QString::number(p->pcb->pid)));
        ui->readyList->setItem(row, 1, new QTableWidgetItem(p->pcb->name));
        ui->readyList->setItem(row, 2, new QTableWidgetItem(QString::number(p->pcb->priority)));
        ui->readyList->setItem(row, 3, new QTableWidgetItem(QString::number(p->pcb->demand_time)));
        ui->readyList->setItem(row, 4, new QTableWidgetItem(QString::number(p->running_time)));
        ui->readyList->setItem(row, 5, new QTableWidgetItem(QString::number(p->size)));
        ui->readyList->setItem(row, 6, new QTableWidgetItem(p->pcb->process_property == INDEPENDENT ? "独立进程" : "同步进程"));

        if (p->pcb->pre_process) {
            ui->readyList->setItem(row, 7, new QTableWidgetItem(QString::number(p->pcb->pre_process->pcb->pid) + ": " + p->pcb->pre_process->pcb->name));
        }
        else {
            ui->readyList->setItem(row, 7, new QTableWidgetItem("---"));
        }

        if (p->pcb->suc_process) {
            ui->readyList->setItem(row, 8, new QTableWidgetItem(QString::number(p->pcb->suc_process->pcb->pid) + ": " + p->pcb->suc_process->pcb->name));
        }
        else {
            ui->readyList->setItem(row, 8, new QTableWidgetItem("---"));
        }

        for (int c = 0; c <= 8; ++c) {
            item = ui->readyList->item(row, c);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void MainWindow::onUpdateBackupList() {
    const int rows = ui->backupList->rowCount();
    for (int i = 0; i< rows; ++i) {
        ui->backupList->removeRow(0);
    }

    QTableWidgetItem* item;
    for (list<Process*>::iterator i = os->backup.begin(); i != os->backup.end(); ++i) {
        int row = ui->backupList->rowCount();
        ui->backupList->insertRow(row);
        ui->backupList->setItem(row, 0, new QTableWidgetItem(QString::number((*i)->pcb->pid)));
        ui->backupList->setItem(row, 1, new QTableWidgetItem((*i)->pcb->name));
        ui->backupList->setItem(row, 2, new QTableWidgetItem(QString::number((*i)->pcb->priority)));
        ui->backupList->setItem(row, 3, new QTableWidgetItem(QString::number((*i)->pcb->demand_time)));
        ui->backupList->setItem(row, 4, new QTableWidgetItem(QString::number((*i)->size)));
        ui->backupList->setItem(row, 5, new QTableWidgetItem((*i)->pcb->process_property == INDEPENDENT ? "独立进程" : "同步进程"));

        if ((*i)->pcb->pre_process) {
            ui->backupList->setItem(row, 6, new QTableWidgetItem(QString::number((*i)->pcb->pre_process->pcb->pid) + ": " + (*i)->pcb->pre_process->pcb->name));
        }
        else {
            ui->backupList->setItem(row, 6, new QTableWidgetItem("---"));
        }

        if ((*i)->pcb->suc_process) {
            ui->backupList->setItem(row, 7, new QTableWidgetItem(QString::number((*i)->pcb->suc_process->pcb->pid) + ": " + (*i)->pcb->suc_process->pcb->name));
        }
        else {
            ui->backupList->setItem(row, 7, new QTableWidgetItem("---"));
        }

        for (int c = 0; c <= 7; ++c) {
            item = ui->backupList->item(row, c);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void MainWindow::onUpdateWaitingList() {
    const int rows = ui->waitingList->rowCount();
    for (int i = 0; i< rows; ++i) {
        ui->waitingList->removeRow(0);
    }

    QTableWidgetItem* item;
    for (list<Process*>::iterator i = os->waiting.begin(); i != os->waiting.end(); ++i) {
        int row = ui->waitingList->rowCount();
        ui->waitingList->insertRow(row);
        ui->waitingList->setItem(row, 0, new QTableWidgetItem(QString::number((*i)->pcb->pid)));
        ui->waitingList->setItem(row, 1, new QTableWidgetItem((*i)->pcb->name));
        ui->waitingList->setItem(row, 2, new QTableWidgetItem(QString::number((*i)->pcb->priority)));
        if ((*i)->io->trigger) {
            ui->waitingList->setItem(row, 3, new QTableWidgetItem(QString::number((*i)->io->time)));
            ui->waitingList->setItem(row, 4, new QTableWidgetItem(QString::number((*i)->io->running_time)));
        }
        else {
            ui->waitingList->setItem(row, 3, new QTableWidgetItem("不进行"));
            ui->waitingList->setItem(row, 4, new QTableWidgetItem("---"));
        }
        ui->waitingList->setItem(row, 5, new QTableWidgetItem(QString::number((*i)->size)));
        ui->waitingList->setItem(row, 6, new QTableWidgetItem((*i)->pcb->process_property == INDEPENDENT ? "独立进程" : "同步进程"));

        if ((*i)->pcb->pre_process) {
            ui->waitingList->setItem(row, 7, new QTableWidgetItem(QString::number((*i)->pcb->pre_process->pcb->pid) + ": " + (*i)->pcb->pre_process->pcb->name));
        }
        else {
            ui->waitingList->setItem(row, 7, new QTableWidgetItem("---"));
        }

        if ((*i)->pcb->suc_process) {
            ui->waitingList->setItem(row, 8, new QTableWidgetItem(QString::number((*i)->pcb->suc_process->pcb->pid) + ": " + (*i)->pcb->suc_process->pcb->name));
        }
        else {
            ui->waitingList->setItem(row, 8, new QTableWidgetItem("---"));
        }

        QPushButton* suspendBtn = new QPushButton;
        suspendBtn->setText("挂起");
        suspendBtn->setFont(QFont("微软雅黑", 10));
        suspendBtn->setMaximumHeight(1000 + (*i)->pcb->pid);
        connect(suspendBtn, SIGNAL(clicked()), this, SLOT(on_suspendBtn_clicked()));
        ui->waitingList->setCellWidget(row, 9, suspendBtn);

        for (int c = 0; c <= 8; ++c) {
            item = ui->waitingList->item(row, c);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void MainWindow::onUpdateSuspendedList() {
    const int rows = ui->suspendedList->rowCount();
    for (int i = 0; i< rows; ++i) {
        ui->suspendedList->removeRow(0);
    }

    QTableWidgetItem* item;
    for (list<Process*>::iterator i = os->suspended.begin(); i != os->suspended.end(); ++i) {
        int row = ui->suspendedList->rowCount();
        ui->suspendedList->insertRow(row);
        ui->suspendedList->setItem(row, 0, new QTableWidgetItem(QString::number((*i)->pcb->pid)));
        ui->suspendedList->setItem(row, 1, new QTableWidgetItem((*i)->pcb->name));
        ui->suspendedList->setItem(row, 2, new QTableWidgetItem(QString::number((*i)->pcb->priority)));
        ui->suspendedList->setItem(row, 3, new QTableWidgetItem(QString::number((*i)->pcb->demand_time)));
        ui->suspendedList->setItem(row, 4, new QTableWidgetItem(QString::number((*i)->running_time)));
        ui->suspendedList->setItem(row, 5, new QTableWidgetItem(QString::number((*i)->size)));
        ui->suspendedList->setItem(row, 6, new QTableWidgetItem((*i)->pcb->process_property == INDEPENDENT ? "独立进程" : "同步进程"));

        if ((*i)->pcb->pre_process) {
            ui->suspendedList->setItem(row, 7, new QTableWidgetItem(QString::number((*i)->pcb->pre_process->pcb->pid) + ": " + (*i)->pcb->pre_process->pcb->name));
        }
        else {
            ui->suspendedList->setItem(row, 7, new QTableWidgetItem("---"));
        }

        if ((*i)->pcb->suc_process) {
            ui->suspendedList->setItem(row, 8, new QTableWidgetItem(QString::number((*i)->pcb->suc_process->pcb->pid) + ": " + (*i)->pcb->suc_process->pcb->name));
        }
        else {
            ui->suspendedList->setItem(row, 8, new QTableWidgetItem("---"));
        }

        QPushButton* cancelSuspendBtn = new QPushButton;
        cancelSuspendBtn->setText("解挂");
        cancelSuspendBtn->setFont(QFont("微软雅黑", 10));
        cancelSuspendBtn->setMaximumHeight(1000 + (*i)->pcb->pid);
        connect(cancelSuspendBtn, SIGNAL(clicked()), this, SLOT(on_cancelSuspendBtn_clicked()));
        ui->suspendedList->setCellWidget(row, 9, cancelSuspendBtn);

        for (int c = 0; c <= 8; ++c) {
            item = ui->suspendedList->item(row, c);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void MainWindow::onUpdateMemoryTable() {
    const int rows = ui->memoryTable->rowCount();
    for (int i = 0; i< rows; ++i) {
        ui->memoryTable->removeRow(0);
    }

    QTableWidgetItem* item;
    for (list<pair<int, int>>::iterator i = os->memory->memory_table.begin(); i != os->memory->memory_table.end(); ++i) {
        int row = ui->memoryTable->rowCount();
        ui->memoryTable->insertRow(row);
        ui->memoryTable->setItem(row, 0, new QTableWidgetItem(QString::number(i->first)));
        ui->memoryTable->setItem(row, 1, new QTableWidgetItem(QString::number(i->second)));
        for (int c = 0; c < 2; ++c) {
            item = ui->memoryTable->item(row, c);
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void MainWindow::on_addProcessBtn_clicked()
{
    newProcessWindow->before_show();
    newProcessWindow->show();
}

void MainWindow::on_startBtn_clicked()
{
    const QString startBtnLabel = ui->startBtn->text();

    if (startBtnLabel == "开始运行") {
        timer->start();
        ui->startBtn->setText("暂停");
        ui->singleBtn->setDisabled(true);
    }
    else if (startBtnLabel == "继续") {
        os->logger->log("系统恢复工作");
        timer->start();
        ui->startBtn->setText("暂停");
        ui->singleBtn->setDisabled(true);
    }
    else {
        os->logger->log("系统暂停工作");
        timer->stop();
        ui->startBtn->setText("继续");
        ui->singleBtn->setDisabled(false);
    }
}

void MainWindow::on_singleBtn_clicked()
{
    onTimeout();
}

void MainWindow::on_suspendBtn_clicked() {
    QPushButton *pushBtn = dynamic_cast<QPushButton*>(this->sender());
    if(pushBtn == nullptr) return;
    const int pid = pushBtn->maximumHeight() - 1000;
    os->suspend(pid);
}

void MainWindow::on_cancelSuspendBtn_clicked() {
    QPushButton *pushBtn = dynamic_cast<QPushButton*>(this->sender());
    if(pushBtn == nullptr) return;
    const int pid = pushBtn->maximumHeight() - 1000;
    if (!os->relieve_suspended(pid)) {
        QMessageBox::warning(this, "进程解挂", "内存不足，暂时无法解挂");
    }
}

