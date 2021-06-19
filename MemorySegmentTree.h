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
	MemoryState state; // ������ǣ�trueΪ�ѷ��䣬falseΪ�ͷ�
public:
	MemorySegmentTree();
	
	friend class Memory;
    friend class MainWindow;
};
