#include "MemoryDisplayBlock.h"
#include "Process_.h"
#include "enums.h"

MemoryDisplayBlock::MemoryDisplayBlock(Process* p, int begin, int width, QColor color, MemoryAllocateFlag flag)
    : pid(p->pcb->pid), name(p->pcb->name), begin(begin), width(width), color(color), status(flag) {}
