#pragma once
#include <queue>
#include <list>
#include "Memory.h"
#include "Process_.h"
#include "Logger.h"
#include "qobject.h"
#include "Core.h"
using namespace std;

class OS : public QObject{
    Q_OBJECT

private:
    Memory* memory; // 内存
	list<Process*> backup; // 后备队列(链表)
	priority_queue<Process*, vector<Process*>, PriorityCompare> ready; // 就绪队列
    list<Process*> waiting; // 等待(阻塞)队列(链表)
    list<Process*> suspended; // 挂起队列
    list<Process*> all; // 全部进程
    queue<Process*> process_added_async; // 异步加入进程暂存链表
	Process* p_running = nullptr; // 当前运行进程指针
    Logger* logger; // 日志
    Core** cores;

    const int os_size = 400;
	const int timeslice = 2;
	const int unit_time = 1000;

	void add_process_sync();
	void load_process_from_backup();
	void wake_process_from_waiting();
    void execute();
    void clearRunningProcess(int);
    void updateReadyList();
    void updateBackupList();
    void updateWaitingList();
    void updateSuspendedList();
public:
    OS(int memory_size = 1024);

    void init();
	void add_process(Process*);
//    void setRunningProcess(int, Process*);
    void suspend(int);
    bool relieve_suspended(int);

    friend class MainWindow;
    friend class NewProcessWindow;
    friend int main(int argc, char *argv[]);

signals:
//    void setRunningProcessSignal(int, Process*);
    void updateReadyListSignal();
    void updateBackupListSignal();
    void updateWaitingListSignal();
    void updateSuspendedListSignal();
    void clearRunningProcessSignal(int);
};
