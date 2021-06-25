#include "Core.h"
#include "Process_.h"
#include <list>
#include "Logger.h"
#include <queue>
#include "Memory.h"
#include <iostream>
using namespace std;

Core::Core(int cid) : cid(cid) {}

void Core::init(int timeslice, priority_queue<Process*, vector<Process*>, PriorityCompare>* ready, list<Process*>* waiting, Memory* memory, Logger* logger) {
    this->timeslice = timeslice;
    this->ready_ptr = ready;
    this->waiting_ptr = waiting;
    this->memory_ptr = memory;
    this->logger_ptr = logger;
}

void Core::execute(Process* p_running) {
    if (p_running) this->p_running = p_running;
    if (this->p_running == nullptr) return;
    logger_ptr->log("CPU" + QString::number(cid) + "开始运行进程 " +this->p_running->pcb->name);
    idle = false;
    if (this->p_running->run(timeslice)) {
        if (this->p_running->pcb->state == WAITING) {
            logger_ptr->log("CPU" + QString::number(cid) + "进程 " + this->p_running->pcb->name + " 开始执行I/O操作");
            waiting_ptr->push_back(this->p_running);
        }else if (this->p_running->pcb->state == TERMINATED) {
            logger_ptr->log("CPU" + QString::number(cid) + "进程 " + this->p_running->pcb->name + " 运行完毕");
            memory_ptr->free(this->p_running);
        } else {
            ready_ptr->push(this->p_running);
        }
        setRunningProcess();

        this->p_running = nullptr;
        idle = true;
    }
    else {
//        this->p_running->pcb->state = READY;
//        ready_ptr->push(this->p_running);
        setRunningProcess();
    }
}

void Core::setRunningProcess() {
    emit setRunningProcessSignal(cid, p_running);
}
