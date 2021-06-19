#include "MemorySegmentTree.h"
#include "enums.h"

MemorySegmentTree::MemorySegmentTree()
	: left(0), right(0), max(0), left_available(0),
	right_available(0), state(STANDBY) {}
