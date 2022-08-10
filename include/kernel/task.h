#pragma once
#include <stddef.h>
struct Context {
    size_t rsp, cr3;
};

struct Task {
    Context ctx;
    int pid;
    int uid;
};

extern "C" void ContextSwitch();
void TaskInitialize();
extern "C" Context SaveAndLoadNextTask(size_t rsp, size_t cr3);
extern "C" void ContextSwitchTo(size_t rsp, size_t cr3);
extern "C" void KeIdle();