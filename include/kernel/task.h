#pragma once
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
extern "C" Context LoadNextTask(size_t rsp, size_t cr3);
extern "C" void ContextSwitchNoSave(size_t rsp, size_t cr3);
extern "C" void KeIdle();