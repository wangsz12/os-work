#include "Process_.h"
#include "Logger.h"
#include "enums.h"
#include <iostream>
#include <cstdlib>
using namespace std;

bool PriorityCompare::operator()(Process*& a, Process*& b) {
    return a->pcb->priority < b->pcb->priority;
}

Process::Process(QString name, int demand_time, int priority, int size)
    : pcb(new PCB(name, demand_time, priority, INDEPENDENT, nullptr, nullptr)), size(size)
{
    set_io();
}

Process::Process(QString name, int demand_time, int priority, int size, Process* pre_process, Process* suc_process)
    : pcb(new PCB(name, demand_time, priority, SYNC, pre_process, suc_process)), size(size)
{
    set_io();
}

void Process::set_io() {
    this->io = new IO;
    this->io->trigger = qrand() % 10 <= 5;
    if (this->io->trigger) {
        this->io->begin_time = qrand() % pcb->demand_time;
        this->io->time = qrand() % 10 + 1;
    }

//    cout << this->pcb->name.toStdString() << ": " << this->io->trigger << " " << this->io->begin_time << " " << this->io->time << endl;
}

bool Process::run(int timeslice) {
    pcb->state = RUNNING;
    ++running_time;

    if (io->trigger && running_time == io->begin_time) {
        pcb->state = WAITING;
        return true;
    }

    if (running_time == pcb->demand_time) {
        pcb->state = TERMINATED;
		return true;
	}

    if (pcb->priority) {
        pcb->priority--;
	}

    if (++continuous_time == timeslice) {
        continuous_time = 0;
        pcb->state = READY;
		return true;
    }

	return false;
}
