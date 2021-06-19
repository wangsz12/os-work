#include "PCB.h"
#include "enums.h"
using namespace std;

int PCB::PID = 0;

PCB::PCB(QString name, int demand_time, int priority, ProcessProperty props, Process* pre_process, Process* suc_process)
    : pid(this->PID++), name(name), demand_time(demand_time), priority(priority), state(NEW), process_property(props),
      pre_process(pre_process), suc_process(suc_process) {}
