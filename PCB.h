#pragma once
#include "enums.h"
#include "qstring.h"

class Process;
class PriorityCompare;

class PCB {
private:
	static int PID;

	int pid; // 进程id
    QString name; // 进程名
	int demand_time; // 运行所需时间
	int priority; // 优先级

	State state; // 进程状态
	ProcessProperty process_property; // 进程属性

    Process* pre_process; // 前驱进程
    Process* suc_process; // 后继进程
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
