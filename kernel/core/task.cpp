#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/panic.h>
#include <kernel/idt.h>
#include <kernel/task.h>

LL<Task>* task_list;
int next_pid=0;

void TaskInitialize() {
    task_list = (LL<Task>*)MemHeapAllocate(sizeof(LL<Task>));
    if(!task_list) KePanic("task");
    task_list->next=task_list;task_list->prev=task_list;
    task_list->data.pid = next_pid++;
    task_list->data.uid = 0;
    PitInitialize(70);
    PicClear(0);
}

extern "C" Context LoadNextTask(size_t rsp, size_t cr3) {
    task_list->data.ctx.rsp = rsp;
    task_list->data.ctx.cr3 = cr3;
    task_list=task_list->next;
    return task_list->data.ctx;
}

void TaskAdd(size_t rsp, size_t cr3) {
    LL<Task>* task = (LL<Task>*)MemHeapAllocate(sizeof(LL<Task>));
    task->data.ctx.rsp = rsp;
    task->data.ctx.cr3 = cr3;
    task->data.pid = next_pid++;
    LL<Task>* next = task_list->next;
    ListLink(task_list,task);
    ListLink(task,next);
}

void TaskExit() {
    LL<Task>* task = task_list;
    task_list=task_list->next;
    MemHeapDeallocate((size_t)ListUnlink(task));
    ContextSwitchNoSave(task_list->data.ctx.rsp,task_list->data.ctx.cr3);
    for(;;) KeIdle();
}