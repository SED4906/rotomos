#include <kernel/core.h>
#include <stddef.h>
context_list* contexts;

/// Switch Task
// Takes a stack pointer and page map.
// Returns a stack pointer and page map.
// Stores the arguments in the current task's structure.
// Traverses to the next element of the list.
// (Meant to be called during a pre-emptive context switch.)
context switch_task(size_t rsp, size_t cr3) {
    contexts->c.rsp = rsp;
    contexts->c.cr3 = cr3;
    contexts = contexts->next;
    return contexts->c;
}
/// Initialize Task Scheduler
// Takes no arguments.
// Returns nothing.
// Sets up a circular list of tasks.
// Context switches to initialize the first item.
// Enables a timer for further pre-emptive switches.
void init_task() {
    contexts = (context_list*)kmalloc(sizeof(context_list));
    contexts->next = contexts;
    context_switch();
    init_pit(20);
    pic_clear_mask(0);
}
/// Add Task
// Takes a stack pointer and page map.
// Returns nothing.
// Adds the task to the list.
void add_task(size_t rsp, size_t cr3) {
    context_list* context = (context_list*)kmalloc(sizeof(context_list));
    context->c.rsp = rsp;
    context->c.cr3 = cr3;
    context->next = contexts->next;
    contexts->next = context;
}
/// Exit Task
// Takes no arguments.
// Does not return.
// Removes the current task from the list.
// Switches to the next task up.
__attribute__((noreturn))
void exit_task() {
    context_list* context = contexts;
    while(context->next != contexts);
    context->next = contexts->next;
    kdemalloc(contexts);
    contexts = context->next;
    context_switch_nosave(contexts->c.rsp,contexts->c.cr3);
    for(;;) hang_idle();
}