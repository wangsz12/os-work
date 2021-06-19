#pragma once
#include "PCB.h"
#include "enums.h"
using namespace std;

class QString;

struct IO {
    bool trigger = false;
    int begin_time = -1;
    int time = -1;
    int running_time = 0;
};

class Process {
private:
    PCB* pcb;
	int size;
    int memory_begin = 0;
	int continuous_time = 0;
    int running_time = 0;
    IO* io;

    void set_io();

public:
    Process(QString, int, int, int);
    Process(QString, int, int, int, Process*, Process*);

	bool run(int);

	friend class PriorityCompare;
	friend class OS;
	friend class Memory;
    friend class MainWindow;
    friend class NewProcessWindow;
    friend class MemoryDisplayBlock;
};

class PriorityCompare {
public:
	bool operator()(Process*&, Process*&);
};
