#ifndef CORE_H
#define CORE_H
#include "qobject.h"
#include <list>
#include "Logger.h"
#include <queue>
using namespace std;

class Process;
class PriorityCompare;
class Memory;

class Core : public QObject
{
    Q_OBJECT
public:
    Core(int);

    void init(int, priority_queue<Process*, vector<Process*>, PriorityCompare>*, list<Process*>*, Memory*, Logger*);
    void execute(Process* p_running = nullptr);

    friend class OS;

private:
    int cid;
    bool idle = true;
    Process* p_running;
    int timeslice;
    priority_queue<Process*, vector<Process*>, PriorityCompare>* ready_ptr;
    list<Process*>* waiting_ptr;
    Memory* memory_ptr;
    Logger* logger_ptr;

    void setRunningProcess();

signals:
    void setRunningProcessSignal(int, Process*);
};

#endif // CORE_H
