#pragma once

enum State {
    NEW = 1, READY, RUNNING, WAITING, SUSPENDED, TERMINATED
};

enum ProcessProperty {
    INDEPENDENT = 10, SYNC
};

enum MemoryState {
    ALLOCATED = 100, AVAILABLE, STANDBY
};

enum MemoryAllocateFlag{
    ALLOCATE = 1000, FREE, NOT_DEFINE
};
