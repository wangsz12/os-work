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
    Memory* memory; // �ڴ�
	list<Process*> backup; // �󱸶���(����)
	priority_queue<Process*, vector<Process*>, PriorityCompare> ready; // ��������
    list<Process*> waiting; // �ȴ�(����)����(����)
    list<Process*> suspended; // �������
    list<Process*> all; // ȫ������
    queue<Process*> process_added_async; // �첽��������ݴ�����
	Process* p_running = nullptr; // ��ǰ���н���ָ��
    Logger* logger; // ��־
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
