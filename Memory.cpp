#include "Memory.h"
#include "MemorySegmentTree.h"
#include "Logger.h"
#include <string>
#include <algorithm>
#include "enums.h"
#include "Process_.h"
#include <iostream>
using namespace std;

Memory::Memory(int size): size(size) {
    this->memory = new MemorySegmentTree[size << 2];
    this->memory_table.push_back(make_pair(1, size));

	build_segment_tree(1, 1, size);
}

void Memory::build_segment_tree(int node, int left, int right) {
	MemorySegmentTree*& m = this->memory;

	m[node].left = left;
	m[node].right = right;
	if (left == right) {
		m[node].max = 1;
		m[node].left_available = 1;
		m[node].right_available = 1;
		return;
	}
	int mid = (left + right) >> 1;
	build_segment_tree(node << 1, left, mid);
	build_segment_tree(node << 1 | 1, mid + 1, right);
	m[node].max = max(max(m[node << 1].max, m[node << 1 | 1].max),
		m[node << 1].right_available + m[node << 1 | 1].left_available);

	m[node].left_available = m[node << 1].left_available;
	if (m[node << 1].left + m[node << 1].left_available - 1 == m[node << 1].right)
		m[node].left_available += m[node << 1 | 1].left_available;

	m[node].right_available = m[node << 1 | 1].right_available;
	if (m[node << 1 | 1].left + m[node << 1 | 1].right_available - 1 == m[node << 1 | 1].right)
		m[node].right_available += m[node << 1].right_available;
}

void Memory::update(int node, int a, int b, MemoryState state) {
	MemorySegmentTree*& m = this->memory;

	if (m[node].left == a && m[node].right == b) {
		m[node].state = state;
		if (state == ALLOCATED) {
			m[node].left_available = m[node].right_available = m[node].max = 0;
		}
		else {
			m[node].left_available = m[node].right_available = m[node].max = b - a + 1;
		}
		return;
	}

	if (m[node].state != STANDBY) this->push_down(node);

	int mid = (m[node].left + m[node].right) >> 1;
	if (b <= mid) this->update(node << 1, a, b, state);
	else if (a > mid) this->update(node << 1 | 1, a, b, state);
	else {
		this->update(node << 1, a, mid, state);
		this->update(node << 1 | 1, mid + 1, b, state);
	}

	m[node].max = max(max(m[node << 1].max, m[node << 1 | 1].max),
		m[node << 1].right_available + m[node << 1 | 1].left_available);

	m[node].left_available = m[node << 1].left_available;
	if (m[node << 1].left + m[node << 1].left_available - 1 == m[node << 1].right)
		m[node].left_available = m[node].left_available + m[node << 1 | 1].left_available;

	m[node].right_available = m[node << 1 | 1].right_available;
	if (m[node << 1 | 1].left + m[node << 1 | 1].right_available - 1 == m[node << 1 | 1].right)
		m[node].right_available = m[node].right_available + m[node << 1].right_available;

	if (m[node << 1].state == m[node << 1 | 1].state) {
		m[node].state = m[node << 1].state;
	}
}

void Memory::push_down(int node) {
	MemorySegmentTree*& m = this->memory;
	m[node << 1].state = m[node << 1 | 1].state = m[node].state;
	if (m[node].state == ALLOCATED) {
		m[node << 1].left_available = m[node << 1].right_available = m[node << 1].max = 0;
		m[node << 1 | 1].right_available = m[node << 1 | 1].right_available = m[node << 1 | 1].max = 0;
	}
	else {
		m[node << 1].left_available = m[node << 1].right_available = m[node << 1].max = m[node << 1].right - m[node << 1].left + 1;
		m[node << 1 | 1].right_available = m[node << 1 | 1].right_available = m[node << 1 | 1].max = m[node << 1 | 1].right - m[node << 1 | 1].left + 1;
	}
	m[node].state = STANDBY;
}

int Memory::find(int node, int demand) {
	MemorySegmentTree*& m = this->memory;

	if (m[node].left == m[node].right) {
		if (m[node].state == AVAILABLE && demand == 1) {
			m[node].state = ALLOCATED;
			return m[node].left;
		}
	}

	if (m[node].state != STANDBY) {
		this->push_down(node);
	}

	if (m[node << 1].max >= demand) {
		return this->find(node << 1, demand);
	}
	if (m[node << 1].right_available + m[node << 1 | 1].left_available >= demand) {
		return m[node << 1].right - m[node << 1].right_available + 1;
	}
	if (m[node << 1 | 1].max >= demand) {
		return this->find(node << 1 | 1, demand);
	}
	return 0;
}

int Memory::allocate(Process* p) {
    int memory_begin = this->find(1, p->size);
	if (memory_begin) {
		p->memory_begin = memory_begin;
        update(1, memory_begin, memory_begin + p->size - 1, ALLOCATED);
        updateMemoryTable(memory_begin, p->size, ALLOCATE);
        used += p->size;
        emit setMemorySignal(p, ALLOCATE);
        return memory_begin;
	}
    return 0;
}

void Memory::free(Process* p) {
    update(1, p->memory_begin, p->memory_begin + p->size - 1, AVAILABLE);
    updateMemoryTable(p->memory_begin, p->size, FREE);
    used -= p->size;
    emit setMemorySignal(p, FREE);
}

bool memory_table_compare(pair<int, int> &lhs, pair<int, int> &rhs) {
    return lhs.first < rhs.first;
}

void Memory::updateMemoryTable(int begin, int size, MemoryAllocateFlag flag) {
    const int end = begin + size - 1;
    list<pair<int, int>> temp;
    list<pair<int, int>>::iterator i = memory_table.begin();
    bool freed = false;
    while(i != memory_table.end()) {
        int table_end = i->first + i->second - 1;

        if (flag == ALLOCATE) {
            if (table_end < begin || i->first > end) {
                ++i;
                continue;
            }

            if (begin > i->first && end <= table_end) {
                temp.push_back(make_pair(i->first, begin - i->first));
            }

            if (begin >= i->first && end < table_end) {
                temp.push_back(make_pair(end + 1, table_end - end));
            }

            i = memory_table.erase(i);
        }
        else {
            if (begin == table_end + 1) {
                temp.push_back(make_pair(i->first, i->second + size));
                i = memory_table.erase(i);
                freed = true;
                break;
            }

            if (end + 1 == i->first) {
                temp.push_back(make_pair(begin, i->second + size));
                i = memory_table.erase(i);
                freed = true;
                break;
            }

            ++i;
        }
    }

    if (flag == FREE && !freed) temp.push_back(make_pair(begin, size));

    for (i = temp.begin(); i != temp.end(); ++i) {
        memory_table.push_back((*i));
    }
    temp.clear();
    memory_table.sort(memory_table_compare);

    int begin_;
    int size_;
    i = memory_table.begin();
    while (i != memory_table.end()) {
        begin_ = i->first;
        size_ = i->second;
        while (next(i) != memory_table.end() && i->first + i->second == next(i)->first) {
            size_ += next(i)->second;
            ++i;
        }
        temp.push_back(make_pair(begin_, size_));
        ++i;
    }

    memory_table.swap(temp);
    emit updateMemoryTableSignal();
}
