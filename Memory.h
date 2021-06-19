#pragma once
#include "MemorySegmentTree.h"
#include "enums.h"
#include "qobject.h"
#include <utility>
using namespace std;

class Process;

class Memory : public QObject {
    Q_OBJECT

private:
	int size;
    int used = 0;
	MemorySegmentTree* memory;
    list<pair<int, int>> memory_table;

	void build_segment_tree(int, int, int);
	void update(int, int, int, MemoryState);
	void push_down(int);
	int find(int, int);
    void updateMemoryTable(int, int, MemoryAllocateFlag);
public:
	Memory(int);

    int allocate(Process*);
	void free(Process*);

    friend int main(int argc, char *argv[]);
    friend class MainWindow;

signals:
    void setMemorySignal(Process*, MemoryAllocateFlag);
    void updateMemoryTableSignal();
};
