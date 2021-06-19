#include "mainwindow.h"
#include "qobject.h"
#include "qstring.h"
#include "enums.h"
#include <QMetaType>
#include "Process_.h"
#include <queue>
#include <QApplication>
#include "newprocesswindow.h"
#include <ctime>
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qsrand(time(NULL));

    qRegisterMetaType<MemoryAllocateFlag>("MemoryAllocateFlag");

    QObject::connect(w.os->logger, SIGNAL(logSignal(QString)), &w, SLOT(onLog(QString)), Qt::QueuedConnection);
    QObject::connect(w.os, SIGNAL(setRunningProcessSignal(Process*)), &w, SLOT(onSetRunningProcess(Process*)), Qt::QueuedConnection);
    QObject::connect(w.os->memory, SIGNAL(setMemorySignal(Process*, MemoryAllocateFlag)), &w, SLOT(onSetMemory(Process*, MemoryAllocateFlag)), Qt::QueuedConnection);
    QObject::connect(w.newProcessWindow, SIGNAL(addProcessSignal(QString, int, int, int, int, Process*, Process*)), &w, SLOT(onAddProcess(QString, int, int, int, int, Process*, Process*)), Qt::QueuedConnection);
    QObject::connect(w.os, SIGNAL(updateReadyListSignal()), &w, SLOT(onUpdateReadyList()), Qt::QueuedConnection);
    QObject::connect(w.os, SIGNAL(updateBackupListSignal()), &w, SLOT(onUpdateBackupList()), Qt::QueuedConnection);
    QObject::connect(w.os, SIGNAL(updateWaitingListSignal()), &w, SLOT(onUpdateWaitingList()), Qt::QueuedConnection);
    QObject::connect(w.os, SIGNAL(updateSuspendedListSignal()), &w, SLOT(onUpdateSuspendedList()), Qt::QueuedConnection);
    QObject::connect(w.os->memory, SIGNAL(updateMemoryTableSignal()), &w, SLOT(onUpdateMemoryTable()), Qt::QueuedConnection);

    w.show();

    w.initWindow();
    w.os->logger->log("系统开始工作...");
    w.os->logger->log("内存设置为 " + QString::number(w.os->memory->size) + " 单位");
    return a.exec();
}
