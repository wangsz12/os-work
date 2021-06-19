#pragma once
#include "enums.h"

class Memory;

class MemorySegmentTree {
private:
	int left;
	int right;
	int max;
	int left_available;
	int right_available;
	MemoryState state; // 操作标记，true为已分配，false为释放
public:
	MemorySegmentTree();
	
	friend class Memory;
    friend class MainWindow;
};
