#pragma once
#include "enums.h"
#include "qstring.h"

class Process;
class PriorityCompare;

class PCB {
private:
	static int PID;

	int pid; // ����id
    QString name; // ������
	int demand_time; // ��������ʱ��
	int priority; // ���ȼ�

	State state; // ����״̬
	ProcessProperty process_property; // ��������

    Process* pre_process; // ǰ������
    Process* suc_process; // ��̽���
public:
    PCB(QString, int, int, ProcessProperty, Process*, Process*);

	friend class Process;
	friend class PriorityCompare;
	friend class OS;
    friend class Core;
    friend class MainWindow;
    friend class NewProcessWindow;
    friend class MemoryDisplayBlock;
};
