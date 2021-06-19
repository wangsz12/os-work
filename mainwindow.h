#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtimer.h"
#include "OS.h"
#include "qpainter.h"
#include "enums.h"
#include "newprocesswindow.h"
#include "MemoryDisplayBlock.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Process;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent *);

    friend int main(int argc, char *argv[]);
    friend class NewProcessWindow;

private:
    Ui::MainWindow *ui;
    QTimer* timer;
    OS* os;
    struct {
        list<MemoryDisplayBlock*> blocks;
        const int totalPixel = 805;
        int which = 0;
        QColor colors[10];
        bool first = true;
    } memoryDrawer;
    NewProcessWindow* newProcessWindow;

    void initWindow();
    void updateSuspendedList();

private slots:
    void onTimeout();
    void onLog(QString);
    void onSetRunningProcess(Process*);
    void onSetMemory(Process*, MemoryAllocateFlag);
    void onAddProcess(QString, int, int, int, int, Process*, Process*);
    void onUpdateReadyList();
    void onUpdateBackupList();
    void onUpdateWaitingList();
    void onUpdateSuspendedList();
    void onUpdateMemoryTable();

    void on_addProcessBtn_clicked();
    void on_startBtn_clicked();
    void on_singleBtn_clicked();
    void on_suspendBtn_clicked();
    void on_cancelSuspendBtn_clicked();

signals:
    void updateSuspendedListSignal();
};
#endif // MAINWINDOW_H
