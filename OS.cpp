#include "OS.h"
#include "Logger.h"
#include <iostream>
#include <list>
#include "enums.h"
#include "Memory.h"
#include <queue>
using namespace std;

class Process;
class PriorityCompare;

OS::OS(int memory_size)
{
    memory = new Memory(memory_size);
    logger = new Logger;
    cores = new Core*[2];
    for (int i = 0; i < 2; ++i) {
        cores[i] = new Core(i + 1);
        cores[i]->init(timeslice, &ready, &waiting, memory, logger);
    }
}

void OS::init() {
    Process* os_p = new Process("OS", 999, 999, os_size);
    memory->allocate(os_p);

    add_process(new Process("test1", 5, 999, 100));
    add_process(new Process("test2", 5, 5, 100));
    Process* pre1 = new Process("pre1", 5, 4, 200);
    add_process(pre1);
    add_process(new Process("pre2", 5, 6, 400));
    add_process(new Process("test4", 5, 5, 150));
    add_process(new Process("sync1", 5, 3, 20, pre1, nullptr));
}

void OS::add_process_sync() {
    while (!process_added_async.empty()) {
        Process* p = process_added_async.front();
        process_added_async.pop();
        backup.push_back(p);
        updateBackupList();
	}
}

void OS::add_process(Process* process) {
    process_added_async.push(process);
    all.push_back(process);
}

void OS::load_process_from_backup() {
    list<Process*>::iterator i = backup.begin();
    while (i != backup.end()) {
        (*i)->memory_begin = memory->allocate(*i);

        Process* p_suc = (*i)->pcb->suc_process;
        if ((*i)->pcb->process_property == SYNC && p_suc) {
            p_suc->pcb->process_property = SYNC;
            p_suc->pcb->pre_process = *i;

            switch (p_suc->pcb->state) {
                case RUNNING:
                    p_running = nullptr;
                case READY: {
                    p_suc->pcb->state = WAITING;
                    waiting.push_back(p_suc);

                    priority_queue<Process*, vector<Process*>, PriorityCompare> temp;
                    while (!ready.empty()) {
                        Process* p = ready.top();
                        ready.pop();
                        if (p->pcb->pid != p_suc->pcb->pid) temp.push(p);
                    }
                    ready = temp;
                    break;
                }
                default: break;
            }
        }

        if ((*i)->memory_begin) {
            logger->log("将进程 " + (*i)->pcb->name + " 装入内存");
            logger->log("占用 " + QString::number((*i)->size) + " 单位内存");

            if ((*i)->pcb->process_property == SYNC && (*i)->pcb->pre_process && (*i)->pcb->pre_process->pcb->state != TERMINATED) {
                (*i)->pcb->state = WAITING;
                waiting.push_back((*i));
            }
            else {
                (*i)->pcb->state = READY;
                ready.push((*i));
            }

            i = backup.erase(i);
		}
        else {
			++i;
        }
    }
    updateReadyList();
    updateBackupList();
    updateWaitingList();
    updateSuspendedList();
}

void OS::wake_process_from_waiting() {
    list<Process*>::iterator i = waiting.begin();
    while (i != waiting.end()) {
        if ((*i)->io->trigger && !(*i)->io->finished) {
            ++(*i)->io->running_time;
            if ((*i)->io->running_time == (*i)->io->time) (*i)->io->finished = true;
            ++i;
            continue;
        }

        if ((*i)->pcb->process_property == SYNC && (*i)->pcb->pre_process && (*i)->pcb->pre_process->pcb->state != TERMINATED) {
            ++i;
            continue;
        }

        (*i)->pcb->state = READY;
        ready.push(*i);
        i = waiting.erase(i);
    }
    updateReadyList();
    updateWaitingList();
}

void OS::execute() {
    Process* p[2] = {nullptr, nullptr};
    int no_work = 0;
    for (int i = 0; i < 2; ++i) {
        if (cores[i]->idle) {
            if (!ready.empty()) {
                p[i] = ready.top();
                ready.pop();
            }
            else {
                clearRunningProcess(i + 1);
                no_work++;
            }
        }
    }
    if (no_work == 2) {
        logger->log("没有可运行的进程");
    }
    else {
        for (int i = 0; i < 2; ++i) {
            cores[i]->execute(p[i]);
        }
    }

    updateReadyList();
    updateWaitingList();
}

void OS::suspend(int pid) {
    list<Process*>::iterator i = waiting.begin();
    while (i != waiting.end()) {
        if ((*i)->pcb->pid == pid) {
            (*i)->pcb->state = SUSPENDED;
            suspended.push_back(*i);
            memory->free(*i);
            logger->log("挂起进程 " + (*i)->pcb->name);
            i = waiting.erase(i);
        }
        else ++i;
    }
    updateSuspendedList();
    updateWaitingList();
}

bool OS::relieve_suspended(int pid) {
    list<Process*>::iterator i = suspended.begin();
    while (i != suspended.end()) {
        if ((*i)->pcb->pid == pid) {
            (*i)->memory_begin = memory->allocate(*i);
            if ((*i)->memory_begin) {
                logger->log("将进程 " + (*i)->pcb->name + " 装入内存");
                logger->log("占用 " + QString::number((*i)->size) + " 单位内存");

                (*i)->pcb->state = WAITING;
                waiting.push_back((*i));
            }
            else return false;

            logger->log("解挂进程 " + (*i)->pcb->name);

            i = suspended.erase(i);
        }
        else ++i;
    }
    updateSuspendedList();
    updateWaitingList();

    return true;
}

void OS::clearRunningProcess(int which) {
    emit clearRunningProcessSignal(which);
}

void OS::updateReadyList() {
    emit updateReadyListSignal();
}

void OS::updateBackupList() {
    emit updateBackupListSignal();
}

void OS::updateWaitingList() {
    emit updateWaitingListSignal();
}

void OS::updateSuspendedList() {
    emit updateSuspendedListSignal();
}
