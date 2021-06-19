#pragma once

#include "enums.h"
#include "qstring.h"
#include "qcolor.h"

class Process;

class MemoryDisplayBlock {
private:
    int pid;
    QString name;
    int begin = 0;
    int width = 805;
    QColor color;
    MemoryAllocateFlag status = NOT_DEFINE;

public:
    MemoryDisplayBlock(Process*, int, int, QColor, MemoryAllocateFlag);

    friend class MainWindow;
};
